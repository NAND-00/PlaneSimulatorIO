
#include "XPLMDisplay.h"
#include "XPLMGraphics.h"
#include <string.h>

#if IBM
	#include <windows.h>
#endif
#if LIN
	#include <GL/gl.h>
#elif __GNUC__
	#include <OpenGL/gl.h>
#else
	#include <GL/gl.h>
#endif

#ifdef WIN32
#include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <setupapi.h>
#include "hidapi.h"
#include <wchar.h>
#include <stdint.h>
#include <math.h>
#include "XPLMDataAccess.h"
#include "XPLMMenus.h"
#include "XPLMProcessing.h"
#include "XPLMPlugin.h"
#include "XPLMUtilities.h"
#include <conio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <process.h>
#include <direct.h>
#include <Shlobj.h>


#ifndef XPLM300
	#error This is made to be compiled against the XPLM300 SDK
#endif

#define MAX_STR 255
#define OUTBUFFER_SIZE 9
#define FEATUREBUFFER_SIZE 6

//hid test ***


/*Dataref variables*/
XPLMDataRef pLandingGearStatus = { NULL };
XPLMDataRef pFlapDeployRatio = { NULL };
XPLMDataRef pElevatorTrimDeployRatio = { NULL };
XPLMDataRef pAutopilotMode = { NULL };
XPLMDataRef pRudderTrimDeployRatio = { NULL };
float LandingGearDeployRatio[10];
float flapsDeployRatio;
float elevatorTrimDeployRatio;
float rudderTrimDeployRatio;
int autopilotMode;


/*OUT data variables*/
uint8_t gearState = 0;
int   DeployStatNose;
int   DeployStatLeft;
int   DeployStatRight;
uint8_t flapsState = 0;
uint16_t elevatorTrimState = 0;
uint16_t rudderTrimState = 0;
uint8_t autopilotState = 0;

/*feature status variables*/
uint8_t featureMode; //is the value of featureBuffer[1] and defines the mode of feature status values featureBuffer[2-5]
/* featureMode 
value - mode:
0 - normal (the first 5 featureBuffer bytes are not used)
1 - flaps servo calibration (featureBuffer[2-3] = flapsServoMinValue, featureBuffer[4-5] = flapsServoRange)
2 - elevator trim servo calibration (featureBuffer[2-3] = elevatorTrimServoMinValue, featureBuffer[4-5] = elevatorTrimServoRange)
3 - rudder trim servo calibration
4 - elevator trim input setting (potentiometer -> featureBuffer[3] = 0 / rotary encoder -> featureBuffer[3] = 1)
5 - elevator trim actuator type setting (unipolar -> featureBuffer[3] = 1 / bipolar -> outBuffer[3] = 2 / none -> featureBuffer[3] = 0)
6 - elevator trim rotary encoder resolution setting
7 - elevator trim unipolar stepper resolution setting
8 - elevator trim bipolar stepper resolution setting
9 - elevator trim bipolar stepper current setting
10 - rudder trim input setting (potentiometer -> featureBuffer[3] = 0 / rotary encoder -> featureBuffer[3] = 1)
11 - rudder trim actuator type setting (unipolar -> featureBuffer[3] = 1 / bipolar -> outBuffer[3] = 2 / none -> featureBuffer[3] = 0)
12 - rudder trim rotary encoder resolution setting
13 - rudder trim unipolar stepper resolution setting
14 - rudder trim bipolar stepper resolution setting
15 - rudder trim bipolar stepper current setting


255 
*/
uint8_t outBuffer[OUTBUFFER_SIZE];
uint8_t featureBuffer[FEATUREBUFFER_SIZE];
uint8_t outReportID = 0x0; //outBuffer[0] value
uint8_t featureReportID = 0x0; //featureBuffer[0] value
uint16_t flapsServoRange = 750;
uint16_t flapsServoMinValue = 1500;
uint16_t elevatorTrimServoRange = 750;
uint16_t elevatorTrimServoMinValue = 1500;
uint16_t rudderTrimServoRange = 750;
uint16_t rudderTrimServoMinValue = 1500;
int elevatorTrimInputType = 0; //0 - potentiometer, 1 - rotary encoder
int elevatorTrimActuatorType = 1; //0 - none, 1 - unipolar stepper, 2 bipolar stepper
int elevatorTrimEncoderResolution = 25;
uint16_t elevatorTrimUnipolarResolution = 2038;
uint16_t elevatorTrimBipolarResolution = 400;
uint16_t elevatorTrimBipolarCurrent = 500;

int rudderTrimInputType = 0; //0 - potentiometer, 1 - rotary encoder
int rudderTrimActuatorType = 1; //0 - none, 1 - unipolar stepper, 2 bipolar stepper
int rudderTrimEncoderResolution = 24;
uint16_t rudderTrimUnipolarResolution = 2038;
uint16_t rudderTrimBipolarResolution = 400;
uint16_t rudderTrimBipolarCurrent = 500;

/*Plugin status variables*/
int servoCalibratingStatus[3] = { 0, 0, 0 };

/*hid variables*/
hid_device* dev = NULL;
int res = 0;
unsigned char buf[65];
uint16_t buf16_t[65];
int i;
int vid = 0x483; // ; 0x765
int pid = 0x572b; //; 0x5020

//hid_device* handle;
//wchar_t wstr[MAX_STR];
//const wchar_t* out = NULL;


//output file values
char	outputPath[255];
static FILE* gOutputFile;

//graphic parameteres
static const char* done_label = "Done";
static const char* move_up_label = "Move up";
static const char* move_down_label = "Move down";
static const char* input_label = "Input:";
static const char* potentiometer_label = "Potentiometer";
static const char* encoder_label = "Rotary Encoder";
static const char* actuator_label = "Stepper:";
static const char* unipolar_stepper_label = "Unipolar stepper motor";
static const char* bipolar_stepper_label = "Bipolar stepper motor";
static const char* none_label = "None";
static const char* encoder_resolution_label = "Encoder Resolution:";
static const char* stepper_resolution_label = "Stepper Resolution:";
static const char* stepper_current_label = "Stepper Current [mA]:";
static const char* up_symbol_label = "+";
static const char* down_symbol_label = "-"; 
static float servo_move_up_lbrt[4]; // left, bottom, right, top
static float servo_move_down_lbrt[4]; // left, bottom, right, top
static float servo_done_lbrt[4]; // left, bottom, right, top
static float trim_input_lbrt[4];
static float trim_actuator_lbrt[4];


static float stepper_M_up_lbrt[4];
static float stepper_M_down_lbrt[4];
static float stepper_C_up_lbrt[4];
static float stepper_C_down_lbrt[4];
static float stepper_X_up_lbrt[4];
static float stepper_X_down_lbrt[4];
static float stepper_I_up_lbrt[4];
static float stepper_I_down_lbrt[4];

static float encoder_X_up_lbrt[4];
static float encoder_X_down_lbrt[4];
static float encoder_I_up_lbrt[4];
static float encoder_I_down_lbrt[4];

static float current_M_up_lbrt[4];
static float current_M_down_lbrt[4];
static float current_C_up_lbrt[4];
static float current_C_down_lbrt[4];
static float current_X_up_lbrt[4];
static float current_X_down_lbrt[4];
static float current_I_up_lbrt[4];
static float current_I_down_lbrt[4];



// An opaque handle to the window we will create
static XPLMWindowID	g_window;

// Callbacks we will register when we create our window
int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void * in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void * in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void * in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void * in_refcon, int losing_focus) { }

void				mainMenuFuncHandler(void* mRef, void* iRef);
void				calibrateServoPosition(void* mRef, void* iRef);
void				trimOptions(void* mRef, void* iRef);

void				updateFeatureBuffer(void);
void				drawCalibrateServo(XPLMWindowID in_window_id, void* in_refcon);
int					handle_mouse_servo_calibration(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon);

void				drawTrimOptions(XPLMWindowID in_window_id, void* in_refcon);
int					handle_mouse_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon);

int					handle_mouse_rudder_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon);

void				receive_main_monitor_bounds(int inMonitorIndex, int inLeftBx, int inTopBx, int inRightBx, int inBottomBx, void* refcon);
void				saveConfig(const char* fileName);
void				readConfig(const char* fileName);

//outBuffer data update functions
void updateGearState(uint8_t* buffer, int writePosition);
void updateFlapsState(uint8_t* buffer, int writePosition);
void updateElevatorTrimState(uint8_t* buffer, int writePosition);
void updateRudderTrimState(uint8_t* buffer, int writePosition);

float	updateOutBuffer(float  inFlightLoop, float inInterval, int  inRelativeToNow, void* inRefcon) {
	if (featureMode == 0)
	{
		updateGearState(outBuffer, 1);
		updateFlapsState(outBuffer, 2);
		updateElevatorTrimState(outBuffer, 3);
		updateRudderTrimState(outBuffer, 6);
	}
	res = hid_write(dev, outBuffer, OUTBUFFER_SIZE);
	return 0.001;
}


static int	coord_in_rect(float x, float y, float* bounds_lbrt) { return ((x >= bounds_lbrt[0]) && (x < bounds_lbrt[2]) && (y < bounds_lbrt[3]) && (y >= bounds_lbrt[1])); }

void				drawButton(float* button_lbrt);

void uint16To8Converter(uint16_t input, uint8_t * output, int writePosition) {
	output[writePosition] = input & 0xff;
	output[writePosition + 1] = (input >> 8);
}

void saveConfig(const char* fileName) {
	XPLMGetSystemPath(outputPath);
	strcat(outputPath, fileName);
	gOutputFile = fopen(outputPath, "w");
	fprintf(gOutputFile, "flapsServoMin= %d , flapsServoRange= %d\n", flapsServoMinValue, flapsServoRange);
	fprintf(gOutputFile, "elevatorTrimServoMin= %d , elevatorTrimServoRange= %d\n", elevatorTrimServoMinValue, elevatorTrimServoRange);
	fprintf(gOutputFile, "rudderTrimServoMin= %d , rudderTrimServoRange= %d\n", rudderTrimServoMinValue, rudderTrimServoRange);
	fprintf(gOutputFile, "eTrimInput= %d , eTrimStepper= %d , eTrimEncoderRes= %d, eTrimUStepperRes= %d, eTrimBStepperRes= %d, eTrimBStepperCurrent= %d\n", elevatorTrimInputType, elevatorTrimActuatorType, elevatorTrimEncoderResolution, elevatorTrimUnipolarResolution, elevatorTrimBipolarResolution, elevatorTrimBipolarCurrent);
	fprintf(gOutputFile, "rTrimInput= %d , rTrimStepper= %d , rTrimEncoderRes= %d, rTrimUStepperRes= %d, rTrimBStepperRes= %d, rTrimBStepperCurrent= %d\n", rudderTrimInputType, rudderTrimActuatorType, rudderTrimEncoderResolution, rudderTrimUnipolarResolution, rudderTrimBipolarResolution, rudderTrimBipolarCurrent);
	fclose(gOutputFile);
}

void readConfig(const char* fileName) {
	XPLMGetSystemPath(outputPath);
	strcat(outputPath, fileName);
	gOutputFile = fopen(outputPath, "r");
	fscanf(gOutputFile, "%*s %d , %*s %d", &flapsServoMinValue, &flapsServoRange);
	featureMode = 1;
	uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
	uint16To8Converter(flapsServoRange, featureBuffer, 4);
	updateFeatureBuffer();
	fscanf(gOutputFile, "%*s %d , %*s %d", &elevatorTrimServoMinValue, &elevatorTrimServoRange);
	featureMode = 2;
	uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
	uint16To8Converter(flapsServoRange, featureBuffer, 4);
	updateFeatureBuffer();
	fscanf(gOutputFile, "%*s %d , %*s %d", &rudderTrimServoMinValue, &rudderTrimServoRange);
	featureMode = 3;
	uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
	uint16To8Converter(flapsServoRange, featureBuffer, 4);
	updateFeatureBuffer();
	fscanf(gOutputFile, "%*s %d , %*s %d , %*s %d , %*s %d , %*s %d , %*s %d", &elevatorTrimInputType, &elevatorTrimActuatorType, &elevatorTrimEncoderResolution, &elevatorTrimUnipolarResolution, &elevatorTrimBipolarResolution, &elevatorTrimBipolarCurrent);
	featureMode = 4;
	featureBuffer[3] = elevatorTrimInputType;
	updateFeatureBuffer();
	featureMode = 5;
	featureBuffer[3] = elevatorTrimActuatorType;
	updateFeatureBuffer();
	featureMode = 6;
	featureBuffer[3] = elevatorTrimEncoderResolution;
	updateFeatureBuffer();
	uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
	featureMode = 7;
	updateFeatureBuffer();
	uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
	featureMode = 8;
	updateFeatureBuffer();
	featureMode = 9;
	uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
	updateFeatureBuffer();
	fscanf(gOutputFile, "%*s %d , %*s %d , %*s %d , %*s %d , %*s %d , %*s %d", &rudderTrimInputType, &rudderTrimActuatorType, &rudderTrimEncoderResolution, &rudderTrimUnipolarResolution, &rudderTrimBipolarResolution, &rudderTrimBipolarCurrent);
	featureMode = 10;
	featureBuffer[3] = rudderTrimInputType;
	updateFeatureBuffer();
	featureMode = 11;
	featureBuffer[3] = rudderTrimActuatorType;
	updateFeatureBuffer();
	fclose(gOutputFile);
	featureMode = 12;
	featureBuffer[3] = rudderTrimEncoderResolution;
	updateFeatureBuffer();
	uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
	featureMode = 13;
	updateFeatureBuffer();
	uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
	featureMode = 14;
	updateFeatureBuffer();
	featureMode = 15;
	uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
	updateFeatureBuffer();
	featureMode = 0;
	updateFeatureBuffer();
}

PLUGIN_API int XPluginStart(
							char *		outName,
							char *		outSig,
							char *		outDesc)
{

	
	strcpy(outName, "CockpitIOPlugin");
	strcpy(outSig, "CockpitIOPlugin");
	strcpy(outDesc, "CockpitIOPlugin");
	
	//start hid test ***
	/*Initialize the hidapi library*/
	res = hid_init();
	dev = hid_open(vid, pid, NULL); // open device
	outBuffer[0] = outReportID;
	featureMode = 0;
	
	pLandingGearStatus = XPLMFindDataRef("sim/flightmodel2/gear/deploy_ratio");
	pFlapDeployRatio = XPLMFindDataRef("sim/flightmodel2/controls/flap_handle_deploy_ratio");
	pElevatorTrimDeployRatio = XPLMFindDataRef("sim/flightmodel2/controls/elevator_trim");
	pRudderTrimDeployRatio = XPLMFindDataRef("sim/flightmodel2/controls/rudder_trim");
	//pAutopilotMode = XPLMFindDataRef("sim/cockpit/autopilot/autopilot_mode");
	pAutopilotMode = XPLMFindDataRef("sim/cockpit2/autopilot/servos_on");

	XPLMGetSystemPath(outputPath);
	strcat(outputPath, "calibrationValues.txt");
	gOutputFile = fopen(outputPath, "r");
	if (!gOutputFile) {
		saveConfig("calibrationValues.txt");
	}
	else {
		readConfig("calibrationValues.txt");
	}
	XPLMMenuID	mainId;
	XPLMMenuID	calibServoId;
	XPLMMenuID	setMaxPosId;
	int			item;

	/*create plugin menu*/
	item = XPLMAppendMenuItem(XPLMFindPluginsMenu(), "CockpitIOPlugin", NULL, 1);
	mainId = XPLMCreateMenu("CockpitIOPlugin", XPLMFindPluginsMenu(), item, mainMenuFuncHandler, NULL);
	item = XPLMAppendMenuItem(mainId, "Calibrate Flaps servo", (void*)"CockpitIOPlugin", 1);
	calibServoId = XPLMCreateMenu("Calibrate Flaps servo", mainId, item, calibrateServoPosition, NULL);
	XPLMAppendMenuItem(calibServoId, "Set min position", (void*)"Calibrate Flaps Servo Min Pos", 1);
	XPLMAppendMenuItem(calibServoId, "Set max position", (void*)"Calibrate Flaps Servo Max Pos", 1);
	
	item = XPLMAppendMenuItem(mainId, "Calibrate Elvator Trim servo", (void*)"CockpitIOPlugin", 1);
	calibServoId = XPLMCreateMenu("Calibrate Elevator Trim servo", mainId, item, calibrateServoPosition, NULL);
	XPLMAppendMenuItem(calibServoId, "Set min position", (void*)"Calibrate Elevator Trim Servo Min Pos", 1);
	XPLMAppendMenuItem(calibServoId, "Set max position", (void*)"Calibrate Elevator Trim Servo Max Pos", 1);

	item = XPLMAppendMenuItem(mainId, "Calibrate Rudder Trim servo", (void*)"CockpitIOPlugin", 1);
	calibServoId = XPLMCreateMenu("Calibrate Rudder Trim servo", mainId, item, calibrateServoPosition, NULL);
	XPLMAppendMenuItem(calibServoId, "Set min position", (void*)"Calibrate Rudder Trim Servo Min Pos", 1);
	XPLMAppendMenuItem(calibServoId, "Set max position", (void*)"Calibrate Rudder Trim Servo Max Pos", 1);

	XPLMAppendMenuItem(mainId, "Elevator Trim options", (void*)"Elevator Trim options", 1);
	XPLMAppendMenuItem(mainId, "Rudder Trim options", (void*)"Rudder Trim options", 1);

	//item = XPLMAppendMenuItem(mainId, "Elevator Trim options", (void*)"CockpitIOPlugin", 1);
	//XPLMCreateMenu("Elevator Trim options", mainId, item, trimOptions, NULL);

	//setMaxPosId = XPLMCreateMenu("Set max position", calibServoId, item, calibrateServoPosition, NULL);
	//XPLMAppendMenuItem(setMaxPosId, "Move up", (void*)"Max move up", 1);
	//XPLMAppendMenuItem(setMaxPosId, "Move down", (void*)"Max move down", 1);



	//end hid test ***
	
	XPLMRegisterFlightLoopCallback(updateOutBuffer, 0.0, NULL);
	XPLMSetFlightLoopCallbackInterval(updateOutBuffer, 0.001, 1, NULL);

	return 1;
}

PLUGIN_API void	XPluginStop(void)
{
	// Since we created the window, we'll be good citizens and clean it up
	res = hid_exit();
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }

//draw functions
void drawButton(float* button_lbrt) {
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glColor4fv(white);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(button_lbrt[0], button_lbrt[3]);
		glVertex2i(button_lbrt[2], button_lbrt[3]);
		glVertex2i(button_lbrt[2], button_lbrt[1]);
		glVertex2i(button_lbrt[0], button_lbrt[1]);
	}
	glEnd();
}


void drawCalibrateServo(XPLMWindowID in_window_id, void* in_refcon)
{
	

	float col_white[] = { 1.0, 1.0, 1.0 };
	XPLMSetGraphicsState(
		0 /* no fog */,
		0 /* 0 texture units */,
		0 /* no lighting */,
		0 /* no alpha testing */,
		1 /* do alpha blend */,
		1 /* do depth testing */,
		0 /* no depth writing */
	);
	// We draw our rudimentary button boxes based on the height of the button text
	int char_height;
	XPLMGetFontDimensions(xplmFont_Proportional, NULL, &char_height, NULL);

	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

	// Draw our buttons
	// Position the move up/move down button in the upper left of the window 
	servo_move_down_lbrt[0] = l + 10;
	servo_move_down_lbrt[3] = t - 15;
	servo_move_down_lbrt[2] = servo_move_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, move_down_label, strlen(move_down_label)) + 8; // *just* wide enough to fit the button text
	servo_move_down_lbrt[1] = servo_move_down_lbrt[3] - (1.25f * char_height); // a bit taller than the button text


	servo_move_up_lbrt[0] = servo_move_down_lbrt[2] + 30;
	servo_move_up_lbrt[1] = servo_move_down_lbrt[1];
	servo_move_up_lbrt[2] = servo_move_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, move_up_label, strlen(move_up_label)) + 8;
	servo_move_up_lbrt[3] = servo_move_down_lbrt[3];

	servo_done_lbrt[0] = servo_move_down_lbrt[2] + (servo_move_up_lbrt[0] - servo_move_down_lbrt[2])/2.0 - XPLMMeasureString(xplmFont_Proportional, done_label, strlen(done_label))/2.0;
	servo_done_lbrt[3] = servo_move_down_lbrt[1] - 15;
	servo_done_lbrt[2] = servo_done_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, done_label, strlen(done_label)) + 8;
	servo_done_lbrt[1] = servo_done_lbrt[3] - (1.25f * char_height);
 
	// Draw the boxes around our rudimentary buttons
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glColor4fv(white);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_move_down_lbrt[0], servo_move_down_lbrt[3]);
		glVertex2i(servo_move_down_lbrt[2], servo_move_down_lbrt[3]);
		glVertex2i(servo_move_down_lbrt[2], servo_move_down_lbrt[1]);
		glVertex2i(servo_move_down_lbrt[0], servo_move_down_lbrt[1]);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_move_up_lbrt[0], servo_move_up_lbrt[3]);
		glVertex2i(servo_move_up_lbrt[2], servo_move_up_lbrt[3]);
		glVertex2i(servo_move_up_lbrt[2], servo_move_up_lbrt[1]);
		glVertex2i(servo_move_up_lbrt[0], servo_move_up_lbrt[1]);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_done_lbrt[0], servo_done_lbrt[3]);
		glVertex2i(servo_done_lbrt[2], servo_done_lbrt[3]);
		glVertex2i(servo_done_lbrt[2], servo_done_lbrt[1]);
		glVertex2i(servo_done_lbrt[0], servo_done_lbrt[1]);
	}
	glEnd();

	// Draw the button text (move down)
	XPLMDrawString(col_white, servo_move_down_lbrt[0] + 4, servo_move_down_lbrt[1] + 4, (char*)move_down_label, NULL, xplmFont_Proportional);

	// Draw the button text (move up)
	XPLMDrawString(col_white, servo_move_up_lbrt[0] + 4, servo_move_up_lbrt[1] + 4, (char*)move_up_label, NULL, xplmFont_Proportional);

	// Draw the button text (move up)
	XPLMDrawString(col_white, servo_done_lbrt[0] + 4, servo_done_lbrt[1] + 4, (char*)done_label, NULL, xplmFont_Proportional);
}

void drawTrimOptions(XPLMWindowID in_window_id, void* in_refcon) {

	
		

	float col_white[] = { 1.0, 1.0, 1.0 };
	XPLMSetGraphicsState(
		0 /* no fog */,
		0 /* 0 texture units */,
		0 /* no lighting */,
		0 /* no alpha testing */,
		1 /* do alpha blend */,
		1 /* do depth testing */,
		0 /* no depth writing */
	);
	int char_height;
	int char_width;
	XPLMGetFontDimensions(xplmFont_Proportional, &char_width, &char_height, NULL);
	const char* button_actuator_label;
	const char* button_input_label;
	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {
		button_input_label = elevatorTrimInputType ? encoder_label : potentiometer_label;
		switch (elevatorTrimActuatorType) {
		case 0:
			button_actuator_label = none_label;
			break;
		case 1:
			button_actuator_label = unipolar_stepper_label;
			break;
		case 2:
			button_actuator_label = bipolar_stepper_label;
			break;
		default:
			button_actuator_label = none_label;
			break;
		}
	}
	else {
		button_input_label = rudderTrimInputType ? encoder_label : potentiometer_label;
		switch (rudderTrimActuatorType) {
		case 0:
			button_actuator_label = none_label;
			break;
		case 1:
			button_actuator_label = unipolar_stepper_label;
			break;
		case 2:
			button_actuator_label = bipolar_stepper_label;
			break;
		default:
			button_actuator_label = none_label;
			break;
		}
	}
	


	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

	trim_input_lbrt[0] = l + 10 + XPLMMeasureString(xplmFont_Proportional, input_label, strlen(input_label)) + 100;
	trim_input_lbrt[3] = t - 15;
	trim_input_lbrt[2] = trim_input_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, button_input_label, strlen(button_input_label)) + 8;
	trim_input_lbrt[1] = trim_input_lbrt[3] - (1.25f * char_height);

	trim_actuator_lbrt[0] = trim_input_lbrt[0];
	trim_actuator_lbrt[3] = trim_input_lbrt[1] - 15;
	trim_actuator_lbrt[2] = trim_actuator_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, button_actuator_label, strlen(button_actuator_label)) + 8;
	trim_actuator_lbrt[1] = trim_actuator_lbrt[3] - (1.25f * char_height);

	encoder_X_up_lbrt[0] = trim_input_lbrt[0]; //- 4 * (XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) + 8)
	encoder_X_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
	encoder_X_up_lbrt[2] = encoder_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label))*0.9f;
	encoder_X_up_lbrt[1] = encoder_X_up_lbrt[3] - (0.75f * char_height);

	encoder_I_up_lbrt[0] = encoder_X_up_lbrt[2];
	encoder_I_up_lbrt[3] = encoder_X_up_lbrt[3];
	encoder_I_up_lbrt[2] = encoder_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	encoder_I_up_lbrt[1] = encoder_X_up_lbrt[1];

	encoder_X_down_lbrt[0] = encoder_X_up_lbrt[0];
	encoder_X_down_lbrt[3] = encoder_X_up_lbrt[1] - (1.25f * char_height);
	encoder_X_down_lbrt[2] = encoder_X_up_lbrt[2];
	encoder_X_down_lbrt[1] = encoder_X_down_lbrt[3] - (0.75f * char_height);

	encoder_I_down_lbrt[0] = encoder_X_down_lbrt[2];
	encoder_I_down_lbrt[3] = encoder_X_down_lbrt[3];
	encoder_I_down_lbrt[2] = encoder_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	encoder_I_down_lbrt[1] = encoder_X_down_lbrt[1];

	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {

		if (elevatorTrimInputType == 0) {//if input is pot, put stepper resolution setting right under stepper type setting
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
		}
		else {
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = encoder_X_down_lbrt[1] - 15;
		}

	}
	else if (!strcmp((char*)in_refcon, "Rudder Trim options")) {

		if (rudderTrimInputType == 0) {//if input is pot, put stepper resolution setting right under stepper type setting
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
		}
		else {
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = encoder_X_down_lbrt[1] - 15;
		}

	}
	stepper_M_up_lbrt[2] = stepper_M_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_M_up_lbrt[1] = stepper_M_up_lbrt[3] - (0.75f * char_height);

	stepper_C_up_lbrt[0] = stepper_M_up_lbrt[2];
	stepper_C_up_lbrt[3] = stepper_M_up_lbrt[3];
	stepper_C_up_lbrt[2] = stepper_C_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_C_up_lbrt[1] = stepper_M_up_lbrt[1];

	stepper_X_up_lbrt[0] = stepper_C_up_lbrt[2];
	stepper_X_up_lbrt[3] = stepper_C_up_lbrt[3];
	stepper_X_up_lbrt[2] = stepper_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_X_up_lbrt[1] = stepper_C_up_lbrt[1];

	stepper_I_up_lbrt[0] = stepper_X_up_lbrt[2];
	stepper_I_up_lbrt[3] = stepper_X_up_lbrt[3];
	stepper_I_up_lbrt[2] = stepper_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_I_up_lbrt[1] = stepper_X_up_lbrt[1];

	stepper_M_down_lbrt[0] = stepper_M_up_lbrt[0];
	stepper_M_down_lbrt[3] = stepper_M_up_lbrt[1] - (1.25f * char_height);
	stepper_M_down_lbrt[2] = stepper_M_up_lbrt[2];
	stepper_M_down_lbrt[1] = stepper_M_down_lbrt[3] - (0.75f * char_height);

	stepper_C_down_lbrt[0] = stepper_M_down_lbrt[2];
	stepper_C_down_lbrt[3] = stepper_M_down_lbrt[3];
	stepper_C_down_lbrt[2] = stepper_C_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_C_down_lbrt[1] = stepper_M_down_lbrt[1];

	stepper_X_down_lbrt[0] = stepper_C_down_lbrt[2];
	stepper_X_down_lbrt[3] = stepper_C_down_lbrt[3];
	stepper_X_down_lbrt[2] = stepper_X_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_X_down_lbrt[1] = stepper_C_down_lbrt[1];

	stepper_I_down_lbrt[0] = stepper_X_down_lbrt[2];
	stepper_I_down_lbrt[3] = stepper_X_down_lbrt[3];
	stepper_I_down_lbrt[2] = stepper_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_I_down_lbrt[1] = stepper_X_down_lbrt[1];

	//current setting buttons
	current_M_up_lbrt[0] = trim_input_lbrt[0];
	current_M_up_lbrt[3] = stepper_M_down_lbrt[1] - 15;
	current_M_up_lbrt[2] = current_M_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_M_up_lbrt[1] = current_M_up_lbrt[3] - (0.75f * char_height);

	current_C_up_lbrt[0] = current_M_up_lbrt[2];
	current_C_up_lbrt[3] = current_M_up_lbrt[3];
	current_C_up_lbrt[2] = current_C_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_C_up_lbrt[1] = current_M_up_lbrt[1];

	current_X_up_lbrt[0] = current_C_up_lbrt[2];
	current_X_up_lbrt[3] = current_C_up_lbrt[3];
	current_X_up_lbrt[2] = current_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_X_up_lbrt[1] = current_C_up_lbrt[1];

	current_I_up_lbrt[0] = current_X_up_lbrt[2];
	current_I_up_lbrt[3] = current_X_up_lbrt[3];
	current_I_up_lbrt[2] = current_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_I_up_lbrt[1] = current_X_up_lbrt[1];

	current_M_down_lbrt[0] = current_M_up_lbrt[0];
	current_M_down_lbrt[3] = current_M_up_lbrt[1] - (1.25f * char_height);
	current_M_down_lbrt[2] = current_M_up_lbrt[2];
	current_M_down_lbrt[1] = current_M_down_lbrt[3] - (0.75f * char_height);

	current_C_down_lbrt[0] = current_M_down_lbrt[2];
	current_C_down_lbrt[3] = current_M_down_lbrt[3];
	current_C_down_lbrt[2] = current_C_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_C_down_lbrt[1] = current_M_down_lbrt[1];

	current_X_down_lbrt[0] = current_C_down_lbrt[2];
	current_X_down_lbrt[3] = current_C_down_lbrt[3];
	current_X_down_lbrt[2] = current_X_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_X_down_lbrt[1] = current_C_down_lbrt[1];

	current_I_down_lbrt[0] = current_X_down_lbrt[2];
	current_I_down_lbrt[3] = current_X_down_lbrt[3];
	current_I_down_lbrt[2] = current_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_I_down_lbrt[1] = current_X_down_lbrt[1];



	// Draw the buttons
	drawButton(trim_input_lbrt);
	drawButton(trim_actuator_lbrt);
	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {
		if (button_input_label == encoder_label) { //if encoder is selected as input, show encoder resolution setting
		//drawButton(encoder_X_up_lbrt);
		//drawButton(encoder_I_up_lbrt);
		//drawButton(encoder_X_down_lbrt);
		//drawButton(encoder_I_down_lbrt);
		//Draw the set encoder resolution text
		XPLMDrawString(col_white, l + 10, encoder_X_up_lbrt[1] + 4 - (1.25f * char_height), (char*)encoder_resolution_label, NULL, xplmFont_Proportional);

		//draw up symbols
		XPLMDrawString(col_white, encoder_X_up_lbrt[0], encoder_X_up_lbrt[1]+2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
		XPLMDrawString(col_white, encoder_I_up_lbrt[0], encoder_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

		//draw down symbols
		XPLMDrawString(col_white, encoder_X_down_lbrt[0] + 3, encoder_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
		XPLMDrawString(col_white, encoder_I_down_lbrt[0] + 3, encoder_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

		//draw encoder resolution value
		if(elevatorTrimEncoderResolution > 9)
			XPLMDrawNumber(col_white, encoder_X_up_lbrt[0]+2, encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimEncoderResolution, 2, 0, 0, xplmFont_Proportional);
		else
			XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 1*(char_width-1), encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimEncoderResolution, 1, 0, 0, xplmFont_Proportional);
	}

		if (elevatorTrimActuatorType != 0) { //if stepper is selected as output - draw its settings
			//drawButton(stepper_M_up_lbrt);
			//drawButton(stepper_C_up_lbrt);
			//drawButton(stepper_X_up_lbrt);
			//drawButton(stepper_I_up_lbrt);

			//draw stepper resolution label
			XPLMDrawString(col_white, l + 10, stepper_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_resolution_label, NULL, xplmFont_Proportional);

			//draw resolution up symbols
			XPLMDrawString(col_white, stepper_M_up_lbrt[0], stepper_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_up_lbrt[0], stepper_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_up_lbrt[0], stepper_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_up_lbrt[0], stepper_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw resolution down symbols
			XPLMDrawString(col_white, stepper_M_down_lbrt[0] + 3, stepper_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_down_lbrt[0] + 3, stepper_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_down_lbrt[0] + 3, stepper_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_down_lbrt[0] + 3, stepper_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw stepper resolution value
			if (elevatorTrimActuatorType == 1) { //if unipolar - show unipolar resolution
				if (elevatorTrimUnipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimUnipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimUnipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}
			else if (elevatorTrimActuatorType == 2) { //if bipolar - show bipolar resolution + current setting
				//draw stepper resolution label
				XPLMDrawString(col_white, l + 10, current_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_current_label, NULL, xplmFont_Proportional);

				//draw resolution up symbols
				XPLMDrawString(col_white, current_M_up_lbrt[0], current_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_up_lbrt[0], current_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_up_lbrt[0], current_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_up_lbrt[0], current_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

				//draw resolution down symbols
				XPLMDrawString(col_white, current_M_down_lbrt[0] + 3, current_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_down_lbrt[0] + 3, current_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_down_lbrt[0] + 3, current_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_down_lbrt[0] + 3, current_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

				if (elevatorTrimBipolarCurrent > 999)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3, current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarCurrent > 99)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 1 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarCurrent > 9)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 2 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 3 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 1, 0, 0, xplmFont_Proportional);


				if (elevatorTrimBipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}

		}
	}
	else if (!strcmp((char*)in_refcon, "Rudder Trim options")) {
		if (button_input_label == encoder_label) { //if encoder is selected as input, show encoder resolution setting
		//drawButton(encoder_X_up_lbrt);
		//drawButton(encoder_I_up_lbrt);
		//drawButton(encoder_X_down_lbrt);
		//drawButton(encoder_I_down_lbrt);
		//Draw the set encoder resolution text
			XPLMDrawString(col_white, l + 10, encoder_X_up_lbrt[1] + 4 - (1.25f * char_height), (char*)encoder_resolution_label, NULL, xplmFont_Proportional);

			//draw up symbols
			XPLMDrawString(col_white, encoder_X_up_lbrt[0], encoder_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_up_lbrt[0], encoder_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw down symbols
			XPLMDrawString(col_white, encoder_X_down_lbrt[0] + 3, encoder_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_down_lbrt[0] + 3, encoder_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw encoder resolution value
			if (elevatorTrimEncoderResolution > 9)
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 2, encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimEncoderResolution, 2, 0, 0, xplmFont_Proportional);
			else
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 1 * (char_width - 1), encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimEncoderResolution, 1, 0, 0, xplmFont_Proportional);
		}

		if (rudderTrimActuatorType != 0) { //if stepper is selected as output - draw its settings
			//drawButton(stepper_M_up_lbrt);
			//drawButton(stepper_C_up_lbrt);
			//drawButton(stepper_X_up_lbrt);
			//drawButton(stepper_I_up_lbrt);

			//draw stepper resolution label
			XPLMDrawString(col_white, l + 10, stepper_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_resolution_label, NULL, xplmFont_Proportional);

			//draw resolution up symbols
			XPLMDrawString(col_white, stepper_M_up_lbrt[0], stepper_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_up_lbrt[0], stepper_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_up_lbrt[0], stepper_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_up_lbrt[0], stepper_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw resolution down symbols
			XPLMDrawString(col_white, stepper_M_down_lbrt[0] + 3, stepper_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_down_lbrt[0] + 3, stepper_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_down_lbrt[0] + 3, stepper_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_down_lbrt[0] + 3, stepper_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw stepper resolution value
			if (rudderTrimActuatorType == 1) { //if unipolar - show unipolar resolution
				if (rudderTrimUnipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimUnipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimUnipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}
			else if (rudderTrimActuatorType == 2) { //if bipolar - show bipolar resolution + current setting
				//draw stepper resolution label
				XPLMDrawString(col_white, l + 10, current_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_current_label, NULL, xplmFont_Proportional);

				//draw resolution up symbols
				XPLMDrawString(col_white, current_M_up_lbrt[0], current_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_up_lbrt[0], current_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_up_lbrt[0], current_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_up_lbrt[0], current_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

				//draw resolution down symbols
				XPLMDrawString(col_white, current_M_down_lbrt[0] + 3, current_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_down_lbrt[0] + 3, current_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_down_lbrt[0] + 3, current_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_down_lbrt[0] + 3, current_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

				if (rudderTrimBipolarCurrent > 999)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3, current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarCurrent > 99)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 1 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarCurrent > 9)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 2 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 3 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 1, 0, 0, xplmFont_Proportional);


				if (rudderTrimBipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}

		}
	}


	//draw choose input button label
	XPLMDrawString(col_white, trim_input_lbrt[0] + 4, trim_input_lbrt[1] + 4, (char*)button_input_label, NULL, xplmFont_Proportional);
	//draw choose actuator button label
	XPLMDrawString(col_white, trim_actuator_lbrt[0] + 4, trim_actuator_lbrt[1] + 4, (char*)button_actuator_label, NULL, xplmFont_Proportional);

	// Draw the choose input text 
	XPLMDrawString(col_white, l+10, trim_input_lbrt[1] + 4, (char*)input_label, NULL, xplmFont_Proportional);
	// Draw the choose actuator text 
	XPLMDrawString(col_white, l + 10, trim_actuator_lbrt[1] + 4, (char*)actuator_label, NULL, xplmFont_Proportional);
	

	

}

//menu function handlers
void mainMenuFuncHandler(void* mRef, void* iRef) {
	if (!strcmp((char*)iRef, "Elevator Trim options")) {
		trimOptions(mRef, iRef);
	}
	else if (!strcmp((char*)iRef, "Rudder Trim options")) {
		trimOptions(mRef, iRef);
}
}

void calibrateServoPosition(void* mRef, void* iRef) {



	//graphical parameters
	int global_desktop_bounds[4]; // left, bottom, right, top
	XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);

	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.left = global_desktop_bounds[0] + 50;
	params.bottom = global_desktop_bounds[1] + 350;
	params.right = global_desktop_bounds[0] + 220;
	params.top = global_desktop_bounds[1] + 430;
	params.visible = 1;
	if (!strcmp((char*)iRef, "Calibrate Flaps Servo Min Pos")) {
		featureMode = 1;
		servoCalibratingStatus[0] = 1; //calibration of min position in action (1)
		outBuffer[2] = 0;
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	if (!strcmp((char*)iRef, "Calibrate Flaps Servo Max Pos")) {
		featureMode = 1;
		servoCalibratingStatus[0] = 2; //calibration of max position in action (2)
		outBuffer[2] = 255;
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	if (!strcmp((char*)iRef, "Calibrate Elevator Trim Servo Min Pos")) {
		featureMode = 2;
		servoCalibratingStatus[1] = 1; //calibration of min position in action (1)
		uint16To8Converter(65535, outBuffer, 4);
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	if (!strcmp((char*)iRef, "Calibrate Elevator Trim Servo Max Pos")) {
		featureMode = 2;
		servoCalibratingStatus[1] = 2; //calibration of max position in action (2)
		uint16To8Converter(0, outBuffer, 4);
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	if (!strcmp((char*)iRef, "Calibrate Rudder Trim Servo Min Pos")) {
		featureMode = 3;
		servoCalibratingStatus[2] = 1; //calibration of min position in action (1)
		uint16To8Converter(65535, outBuffer, 7);
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	if (!strcmp((char*)iRef, "Calibrate Rudder Trim Servo Max Pos")) {
		featureMode = 3;
		servoCalibratingStatus[2] = 2; //calibration of max position in action (2)
		uint16To8Converter(0, outBuffer, 7);
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		params.drawWindowFunc = drawCalibrateServo;
	}
	params.handleMouseClickFunc = handle_mouse_servo_calibration;
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	params.refcon = NULL;
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = 1;


	g_window = XPLMCreateWindowEx(&params);

	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	XPLMSetWindowGravity(g_window, 0, 1, 0, 1); // As the X-Plane window resizes, keep our size constant, and our left and top edges in the same place relative to the window's left/top
	XPLMSetWindowResizingLimits(g_window, 170, 80, 200, 200); // Limit resizing our window: maintain a minimum width/height of 200 boxels and a max width/height of 500
	if (!strcmp((char*)iRef, "Calibrate Flaps Servo Min Pos")) {
		XPLMSetWindowTitle(g_window, "Set FL min position");
	}
	if (!strcmp((char*)iRef, "Calibrate Flaps Servo Max Pos")) {
		XPLMSetWindowTitle(g_window, "Set FL max position");
	}
	if (!strcmp((char*)iRef, "Calibrate Elevator Trim Servo Min Pos")) {
		XPLMSetWindowTitle(g_window, "Set ET min position");
	}
	if (!strcmp((char*)iRef, "Calibrate Elevator Trim Servo Max Pos")) {
		XPLMSetWindowTitle(g_window, "Set ET max position");
	}
	if (!strcmp((char*)iRef, "Calibrate Rudder Trim Servo Min Pos")) {
		XPLMSetWindowTitle(g_window, "Set RT min position");
	}
	if (!strcmp((char*)iRef, "Calibrate Rudder Trim Servo Max Pos")) {
		XPLMSetWindowTitle(g_window, "Set RT max position");
	}




/*
	if (!strcmp((char*)iRef, "Max move up"))
	{
		flapsServoRange += 1;
		outBuffer[4] = flapsServoRange >> 8;
		outBuffer[3] = flapsServoRange & 255;
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
		
	}
	if (!strcmp((char*)iRef, "Calibrate Flaps Servo Min Pos"))
	{
		flapsServoRange -= 1;
		outBuffer[4] = flapsServoRange >> 8;
		outBuffer[3] = flapsServoRange & 255;
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
	}
	*/
}

void trimOptions(void* mRef, void* iRef) {

	//graphical parameters
	int global_desktop_bounds[4]; // left, bottom, right, top
	XPLMGetScreenBoundsGlobal(&global_desktop_bounds[0], &global_desktop_bounds[3], &global_desktop_bounds[2], &global_desktop_bounds[1]);
	XPLMCreateWindow_t params;
	params.structSize = sizeof(params);
	params.left = global_desktop_bounds[0] + 200;
	params.bottom = global_desktop_bounds[1] + 250;
	params.right = global_desktop_bounds[0] + 450;
	params.top = global_desktop_bounds[1] + 450;
	params.visible = 1;
	params.drawWindowFunc = drawTrimOptions;
	if (!strcmp((char*)iRef, "Elevator Trim options")) {
		params.refcon = (char*)"Elevator Trim options";
		params.handleMouseClickFunc = handle_mouse_trim_options;
	}
	if (!strcmp((char*)iRef, "Rudder Trim options")) {
		params.refcon = (char*)"Rudder Trim options";
		params.handleMouseClickFunc = handle_mouse_rudder_trim_options;
	}
	params.handleRightClickFunc = dummy_mouse_handler;
	params.handleMouseWheelFunc = dummy_wheel_handler;
	params.handleKeyFunc = dummy_key_handler;
	params.handleCursorFunc = dummy_cursor_status_handler;
	
	params.layer = xplm_WindowLayerFloatingWindows;
	params.decorateAsFloatingWindow = 1;

	/*
	if (!strcmp((char*)iRef, "Elevator Trim options")) {
		featureMode = 4;
		outBuffer[3] = elevatorTrimInputType;
		outBuffer[1] = featureMode;
		hid_write(dev, outBuffer, OUTBUFFER_SIZE);
	}
	*/
	g_window = XPLMCreateWindowEx(&params);

	XPLMSetWindowPositioningMode(g_window, xplm_WindowPositionFree, -1);
	XPLMSetWindowGravity(g_window, 0, 1, 0, 1); // As the X-Plane window resizes, keep our size constant, and our left and top edges in the same place relative to the window's left/top
	XPLMSetWindowResizingLimits(g_window, 300, 200, 350, 200); // Limit resizing our window
	XPLMSetWindowTitle(g_window, (char*)iRef);
}


//mouse handlers
int	handle_mouse_servo_calibration(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus is_down, void* in_refcon) {
	
	if (is_down == xplm_MouseDown)
	{
		if (!XPLMIsWindowInFront(in_window_id))
		{
			XPLMBringWindowToFront(in_window_id);
		}
		//calibrating flaps servo
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[0] == 1) // user sets min position and clicked the move down 
		{
			flapsServoMinValue += 20;
			flapsServoRange -= 20;
			uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
			uint16To8Converter(flapsServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[0] == 1) // user sets min position and clicked the move up
		{
			flapsServoMinValue -= 20;
			flapsServoRange += 20;
			uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
			uint16To8Converter(flapsServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[0] == 2) // user sets max position and clicked the move down 
		{
			flapsServoRange += 20;
			uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
			uint16To8Converter(flapsServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[0] == 2) // user sets max position and clicked the move up
		{
			flapsServoRange -= 20;
			uint16To8Converter(flapsServoMinValue, featureBuffer, 2);
			uint16To8Converter(flapsServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
	//calibrating elevator trim servo
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[1] == 1) // user sets min position and clicked the move down 
		{
			elevatorTrimServoMinValue += 20;
			elevatorTrimServoRange -= 20;
			uint16To8Converter(elevatorTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(elevatorTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[1] == 1) // user sets min position and clicked the move up
		{
			elevatorTrimServoMinValue -= 20;
			elevatorTrimServoRange += 20;
			uint16To8Converter(elevatorTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(elevatorTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[1] == 2) // user sets max position and clicked the move down 
		{
			elevatorTrimServoRange += 20;
			uint16To8Converter(elevatorTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(elevatorTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[1] == 2) // user sets max position and clicked the move up
		{
			elevatorTrimServoRange -= 20;
			uint16To8Converter(elevatorTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(elevatorTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		//calibrating rudder trim servo
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[2] == 1) // user sets min position and clicked the move down 
		{
			rudderTrimServoMinValue += 20;
			rudderTrimServoRange -= 20;
			uint16To8Converter(rudderTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(rudderTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[2] == 1) // user sets min position and clicked the move up
		{
			rudderTrimServoMinValue -= 20;
			rudderTrimServoRange += 20;
			uint16To8Converter(rudderTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(rudderTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_down_lbrt) && servoCalibratingStatus[2] == 2) // user sets max position and clicked the move down 
		{
			rudderTrimServoRange += 20;
			uint16To8Converter(rudderTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(rudderTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}
		else if (coord_in_rect(x, y, servo_move_up_lbrt) && servoCalibratingStatus[2] == 2) // user sets max position and clicked the move up
		{
			rudderTrimServoRange -= 20;
			uint16To8Converter(rudderTrimServoMinValue, featureBuffer, 2);
			uint16To8Converter(rudderTrimServoRange, featureBuffer, 4);
			updateFeatureBuffer();
		}



		else if (coord_in_rect(x, y, servo_done_lbrt))
		{
			saveConfig("calibrationValues.txt");
			featureMode = 0;
			XPLMDestroyWindow(in_window_id);
		}
	}
	return 1;
}

int	handle_mouse_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon) {
	if (is_down == xplm_MouseDown)
	{
		if (!XPLMIsWindowInFront(in_window_id))
		{
			XPLMBringWindowToFront(in_window_id);
		}
		//encoder/pot setting
		else if (coord_in_rect(x, y, trim_input_lbrt) && elevatorTrimInputType == 1) // change input from encoder to potentiometer
		{
			elevatorTrimInputType = 0;
			featureMode = 4;
			featureBuffer[3] = elevatorTrimInputType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_input_lbrt) && elevatorTrimInputType == 0) // change input from potentiometer to encoder
		{
			elevatorTrimInputType = 1;
			featureMode = 4;
			featureBuffer[3] = elevatorTrimInputType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//stepper type setting
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && elevatorTrimActuatorType == 0) // change input from none to unipolar stepper
		{
			elevatorTrimActuatorType = 1;
			featureMode = 5;
			featureBuffer[3] = elevatorTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && elevatorTrimActuatorType == 1) // change input from unipolar stepper to bipolar stepper
		{
			elevatorTrimActuatorType = 2;
			featureMode = 5;
			featureBuffer[3] = elevatorTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && elevatorTrimActuatorType == 2) // change input from bipolar stepper to none
		{
			elevatorTrimActuatorType = 0;
			featureMode = 5;
			featureBuffer[3] = elevatorTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//encoder resolution setting
		else if (coord_in_rect(x, y, encoder_X_up_lbrt) && elevatorTrimInputType == 1) { //add 10 to encoder resoltution
			if (elevatorTrimEncoderResolution < 90)
				elevatorTrimEncoderResolution += 10;
			else 
				elevatorTrimEncoderResolution -= 90;
			featureMode = 6;
			featureBuffer[3] = elevatorTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_I_up_lbrt) && elevatorTrimInputType == 1) { //add 1 to encoder resoltution
			if (elevatorTrimEncoderResolution < 98)
				elevatorTrimEncoderResolution += 1;
			else 
				elevatorTrimEncoderResolution = 0;
			featureMode = 6;
			featureBuffer[3] = elevatorTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_X_down_lbrt) && elevatorTrimInputType == 1) { //substract 10 from encoder resoltution
			if (elevatorTrimEncoderResolution > 9)
				elevatorTrimEncoderResolution -= 10;
			else 
				elevatorTrimEncoderResolution += 90;
			featureMode = 6;
			featureBuffer[3] = elevatorTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_I_down_lbrt) && elevatorTrimInputType == 1) { //substrqact 1 from encoder resoltution
			if (elevatorTrimEncoderResolution > 1)
				elevatorTrimEncoderResolution -= 1;
			else 
				elevatorTrimEncoderResolution = 99;
			featureMode = 6;
			featureBuffer[3] = elevatorTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//stepper resolution setting
		else if (coord_in_rect(x, y, stepper_M_up_lbrt)) { 
			if (elevatorTrimActuatorType == 1) {//add 1000 to unipolar resoltution
				if (elevatorTrimUnipolarResolution < 9000)
					elevatorTrimUnipolarResolution += 1000;
				else 
					elevatorTrimUnipolarResolution -= 9000;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) {//add 1000 to bipolar resoltution
				if (elevatorTrimBipolarResolution < 9000)
					elevatorTrimBipolarResolution += 1000;
				else
					elevatorTrimBipolarResolution -= 9000;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_C_up_lbrt)) { 
			if (elevatorTrimActuatorType == 1) {//add 100 to unipolar resoltution
				if (elevatorTrimUnipolarResolution < 9900)
					elevatorTrimUnipolarResolution += 100;
				else 
					elevatorTrimUnipolarResolution -= 9900;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) {//add 100 to bipolar resoltution
				if (elevatorTrimBipolarResolution < 9900)
					elevatorTrimBipolarResolution += 100;
				else
					elevatorTrimBipolarResolution -= 9900;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_X_up_lbrt)) { 
		if (elevatorTrimActuatorType == 1) {//add 10 to unipolar resoltution
			if (elevatorTrimUnipolarResolution < 9990)
				elevatorTrimUnipolarResolution += 10;
			else 
				elevatorTrimUnipolarResolution -= 9990;
			uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
			featureMode = 7;
		}
		else if (elevatorTrimActuatorType == 2) {//add 10 to bipolar resoltution
			if (elevatorTrimBipolarResolution < 9990)
				elevatorTrimBipolarResolution += 10;
			else
				elevatorTrimBipolarResolution -= 9990;
			uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
			featureMode = 8;
		}
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_I_up_lbrt)) { 
		if (elevatorTrimActuatorType == 1) {//add 1 to unipolar resoltution
			if (elevatorTrimUnipolarResolution < 9998)
				elevatorTrimUnipolarResolution += 1;
			else 
				elevatorTrimUnipolarResolution = 0;
			uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
			featureMode = 7;
		}
		else if (elevatorTrimActuatorType == 2) {//add 1 to bipolar resoltution
			if (elevatorTrimBipolarResolution < 9998)
				elevatorTrimBipolarResolution += 1;
			else
				elevatorTrimBipolarResolution = 0;
			uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
			featureMode = 8;
		}
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_M_down_lbrt)) { 
			if (elevatorTrimActuatorType == 1) {//substract 1000 from unipolar resoltution
				if (elevatorTrimUnipolarResolution > 999)
					elevatorTrimUnipolarResolution -= 1000;
				else
					elevatorTrimUnipolarResolution += 9000;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) {//substract 1000 from bipolar resoltution
				if (elevatorTrimBipolarResolution > 999)
					elevatorTrimBipolarResolution -= 1000;
				else
					elevatorTrimBipolarResolution += 9000;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_C_down_lbrt)) {
			if (elevatorTrimActuatorType == 1) { //substract 100 from unipolar resoltution
				if (elevatorTrimUnipolarResolution > 99)
					elevatorTrimUnipolarResolution -= 100;
				else
					elevatorTrimUnipolarResolution += 900;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) { //substract 100 from biplar resoltution
				if (elevatorTrimBipolarResolution > 99)
					elevatorTrimBipolarResolution -= 100;
				else
					elevatorTrimBipolarResolution += 900;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_X_down_lbrt)) { 
			if (elevatorTrimActuatorType == 1) {//substract 10 from unipolar resoltution
				if (elevatorTrimUnipolarResolution > 9)
					elevatorTrimUnipolarResolution -= 10;
				else
					elevatorTrimUnipolarResolution += 90;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) {//substract 10 from bipolar resoltution
				if (elevatorTrimBipolarResolution > 9)
					elevatorTrimBipolarResolution -= 10;
				else
					elevatorTrimBipolarResolution += 90;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_I_down_lbrt)) { 
			if (elevatorTrimActuatorType == 1) {//substract 1 from unipolar resoltution
				if (elevatorTrimUnipolarResolution > 1)
					elevatorTrimUnipolarResolution -= 1;
				else
					elevatorTrimUnipolarResolution += 9999;
				uint16To8Converter(elevatorTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 7;
			}
			else if (elevatorTrimActuatorType == 2) {//substract 1 from bipolar resoltution
				if (elevatorTrimBipolarResolution > 1)
					elevatorTrimBipolarResolution -= 1;
				else
					elevatorTrimBipolarResolution += 9999;
				uint16To8Converter(elevatorTrimBipolarResolution, featureBuffer, 2);
				featureMode = 8;
			}
		updateFeatureBuffer();
		featureMode = 0;
		}

		//stepper current setting
		else if (coord_in_rect(x, y, current_M_up_lbrt)) {
		//add 1000 to bipolar current
			if (elevatorTrimBipolarCurrent < 9000)
				elevatorTrimBipolarCurrent += 1000;
			else
				elevatorTrimBipolarCurrent -= 9000;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_C_up_lbrt)) {
		//add 100 to bipolar current
		if (elevatorTrimBipolarCurrent < 9900)
			elevatorTrimBipolarCurrent += 100;
		else
			elevatorTrimBipolarCurrent -= 9900;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_X_up_lbrt)) {
		//add 10 to bipolar current
		if (elevatorTrimBipolarCurrent < 9990)
			elevatorTrimBipolarCurrent += 10;
		else
			elevatorTrimBipolarCurrent -= 9990;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_I_up_lbrt)) {
		//add 1 to bipolar current
		if (elevatorTrimBipolarCurrent < 9998)
			elevatorTrimBipolarCurrent += 1;
		else
			elevatorTrimBipolarCurrent = 0;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_M_down_lbrt)) {
		//substract 1000 from bipolar current
		if (elevatorTrimBipolarCurrent > 999)
			elevatorTrimBipolarCurrent -= 1000;
		else
			elevatorTrimBipolarCurrent += 9000;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_C_down_lbrt)) {
		//substract 100 from bipolar current
		if (elevatorTrimBipolarCurrent > 99)
			elevatorTrimBipolarCurrent -= 100;
		else
			elevatorTrimBipolarCurrent += 900;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_X_down_lbrt)) {
		//substract 10 from bipolar current
		if (elevatorTrimBipolarCurrent > 9)
			elevatorTrimBipolarCurrent -= 10;
		else
			elevatorTrimBipolarCurrent += 90;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_I_down_lbrt)) {
		//substract 1 from bipolar current
		if (elevatorTrimBipolarCurrent > 1)
			elevatorTrimBipolarCurrent -= 1;
		else
			elevatorTrimBipolarCurrent += 9999;
		featureMode = 9;
		uint16To8Converter(elevatorTrimBipolarCurrent, featureBuffer, 2);
		updateFeatureBuffer();
		featureMode = 0;
		}
		saveConfig("calibrationValues.txt");
	}
	return 1;
}

int	handle_mouse_rudder_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon) {
	if (is_down == xplm_MouseDown)
	{
		if (!XPLMIsWindowInFront(in_window_id))
		{
			XPLMBringWindowToFront(in_window_id);
		}
		//encoder/pot setting
		else if (coord_in_rect(x, y, trim_input_lbrt) && rudderTrimInputType == 1) // change input from encoder to potentiometer
		{
			rudderTrimInputType = 0;
			featureMode = 10;
			featureBuffer[3] = rudderTrimInputType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_input_lbrt) && rudderTrimInputType == 0) // change input from potentiometer to encoder
		{
			rudderTrimInputType = 1;
			featureMode = 10;
			featureBuffer[3] = rudderTrimInputType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//stepper type setting
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && rudderTrimActuatorType == 0) // change input from none to unipolar stepper
		{
			rudderTrimActuatorType = 1;
			featureMode = 11;
			featureBuffer[3] = rudderTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && rudderTrimActuatorType == 1) // change input from unipolar stepper to bipolar stepper
		{
			rudderTrimActuatorType = 2;
			featureMode = 11;
			featureBuffer[3] = rudderTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, trim_actuator_lbrt) && rudderTrimActuatorType == 2) // change input from bipolar stepper to none
		{
			rudderTrimActuatorType = 0;
			featureMode = 11;
			featureBuffer[3] = rudderTrimActuatorType;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//encoder resolution setting
		else if (coord_in_rect(x, y, encoder_X_up_lbrt) && rudderTrimInputType == 1) { //add 10 to encoder resoltution
			if (rudderTrimEncoderResolution < 90)
				rudderTrimEncoderResolution += 10;
			else
				rudderTrimEncoderResolution -= 90;
			featureMode = 12;
			featureBuffer[3] = rudderTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_I_up_lbrt) && rudderTrimInputType == 1) { //add 1 to encoder resoltution
			if (rudderTrimEncoderResolution < 98)
				rudderTrimEncoderResolution += 1;
			else
				rudderTrimEncoderResolution = 0;
			featureMode = 12;
			featureBuffer[3] = rudderTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_X_down_lbrt) && rudderTrimInputType == 1) { //substract 10 from encoder resoltution
			if (rudderTrimEncoderResolution > 9)
				rudderTrimEncoderResolution -= 10;
			else
				rudderTrimEncoderResolution += 90;
			featureMode = 12;
			featureBuffer[3] = rudderTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, encoder_I_down_lbrt) && rudderTrimInputType == 1) { //substrqact 1 from encoder resoltution
			if (rudderTrimEncoderResolution > 1)
				rudderTrimEncoderResolution -= 1;
			else
				rudderTrimEncoderResolution = 99;
			featureMode = 12;
			featureBuffer[3] = rudderTrimEncoderResolution;
			updateFeatureBuffer();
			featureMode = 0;
		}
		//stepper resolution setting
		else if (coord_in_rect(x, y, stepper_M_up_lbrt)) {
			if (rudderTrimActuatorType == 1) {//add 1000 to unipolar resoltution
				if (rudderTrimUnipolarResolution < 9000)
					rudderTrimUnipolarResolution += 1000;
				else
					rudderTrimUnipolarResolution -= 9000;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//add 1000 to bipolar resoltution
				if (rudderTrimBipolarResolution < 9000)
					rudderTrimBipolarResolution += 1000;
				else
					rudderTrimBipolarResolution -= 9000;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_C_up_lbrt)) {
			if (rudderTrimActuatorType == 1) {//add 100 to unipolar resoltution
				if (rudderTrimUnipolarResolution < 9900)
					rudderTrimUnipolarResolution += 100;
				else
					rudderTrimUnipolarResolution -= 9900;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//add 100 to bipolar resoltution
				if (rudderTrimBipolarResolution < 9900)
					rudderTrimBipolarResolution += 100;
				else
					rudderTrimBipolarResolution -= 9900;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_X_up_lbrt)) {
			if (rudderTrimActuatorType == 1) {//add 10 to unipolar resoltution
				if (rudderTrimUnipolarResolution < 9990)
					rudderTrimUnipolarResolution += 10;
				else
					rudderTrimUnipolarResolution -= 9990;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//add 10 to bipolar resoltution
				if (rudderTrimBipolarResolution < 9990)
					rudderTrimBipolarResolution += 10;
				else
					rudderTrimBipolarResolution -= 9990;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_I_up_lbrt)) {
			if (rudderTrimActuatorType == 1) {//add 1 to unipolar resoltution
				if (rudderTrimUnipolarResolution < 9998)
					rudderTrimUnipolarResolution += 1;
				else
					rudderTrimUnipolarResolution = 0;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//add 1 to bipolar resoltution
				if (rudderTrimBipolarResolution < 9998)
					rudderTrimBipolarResolution += 1;
				else
					rudderTrimBipolarResolution = 0;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_M_down_lbrt)) {
			if (rudderTrimActuatorType == 1) {//substract 1000 from unipolar resoltution
				if (rudderTrimUnipolarResolution > 999)
					rudderTrimUnipolarResolution -= 1000;
				else
					rudderTrimUnipolarResolution += 9000;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//substract 1000 from bipolar resoltution
				if (rudderTrimBipolarResolution > 999)
					rudderTrimBipolarResolution -= 1000;
				else
					rudderTrimBipolarResolution += 9000;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_C_down_lbrt)) {
			if (rudderTrimActuatorType == 1) { //substract 100 from unipolar resoltution
				if (rudderTrimUnipolarResolution > 99)
					rudderTrimUnipolarResolution -= 100;
				else
					rudderTrimUnipolarResolution += 900;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) { //substract 100 from biplar resoltution
				if (rudderTrimBipolarResolution > 99)
					rudderTrimBipolarResolution -= 100;
				else
					rudderTrimBipolarResolution += 900;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_X_down_lbrt)) {
			if (rudderTrimActuatorType == 1) {//substract 10 from unipolar resoltution
				if (rudderTrimUnipolarResolution > 9)
					rudderTrimUnipolarResolution -= 10;
				else
					rudderTrimUnipolarResolution += 90;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//substract 10 from bipolar resoltution
				if (rudderTrimBipolarResolution > 9)
					rudderTrimBipolarResolution -= 10;
				else
					rudderTrimBipolarResolution += 90;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, stepper_I_down_lbrt)) {
			if (rudderTrimActuatorType == 1) {//substract 1 from unipolar resoltution
				if (rudderTrimUnipolarResolution > 1)
					rudderTrimUnipolarResolution -= 1;
				else
					rudderTrimUnipolarResolution += 9999;
				uint16To8Converter(rudderTrimUnipolarResolution, featureBuffer, 2);
				featureMode = 13;
			}
			else if (rudderTrimActuatorType == 2) {//substract 1 from bipolar resoltution
				if (rudderTrimBipolarResolution > 1)
					rudderTrimBipolarResolution -= 1;
				else
					rudderTrimBipolarResolution += 9999;
				uint16To8Converter(rudderTrimBipolarResolution, featureBuffer, 2);
				featureMode = 14;
			}
			updateFeatureBuffer();
			featureMode = 0;
		}

		//stepper current setting
		else if (coord_in_rect(x, y, current_M_up_lbrt)) {
			//add 1000 to bipolar current
			if (rudderTrimBipolarCurrent < 9000)
				rudderTrimBipolarCurrent += 1000;
			else
				rudderTrimBipolarCurrent -= 9000;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_C_up_lbrt)) {
			//add 100 to bipolar current
			if (rudderTrimBipolarCurrent < 9900)
				rudderTrimBipolarCurrent += 100;
			else
				rudderTrimBipolarCurrent -= 9900;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_X_up_lbrt)) {
			//add 10 to bipolar current
			if (rudderTrimBipolarCurrent < 9990)
				rudderTrimBipolarCurrent += 10;
			else
				rudderTrimBipolarCurrent -= 9990;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_I_up_lbrt)) {
			//add 1 to bipolar current
			if (rudderTrimBipolarCurrent < 9998)
				rudderTrimBipolarCurrent += 1;
			else
				rudderTrimBipolarCurrent = 0;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_M_down_lbrt)) {
			//substract 1000 from bipolar current
			if (rudderTrimBipolarCurrent > 999)
				rudderTrimBipolarCurrent -= 1000;
			else
				rudderTrimBipolarCurrent += 9000;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_C_down_lbrt)) {
			//substract 100 from bipolar current
			if (rudderTrimBipolarCurrent > 99)
				rudderTrimBipolarCurrent -= 100;
			else
				rudderTrimBipolarCurrent += 900;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_X_down_lbrt)) {
			//substract 10 from bipolar current
			if (rudderTrimBipolarCurrent > 9)
				rudderTrimBipolarCurrent -= 10;
			else
				rudderTrimBipolarCurrent += 90;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		else if (coord_in_rect(x, y, current_I_down_lbrt)) {
			//substract 1 from bipolar current
			if (rudderTrimBipolarCurrent > 1)
				rudderTrimBipolarCurrent -= 1;
			else
				rudderTrimBipolarCurrent += 9999;
			featureMode = 15;
			uint16To8Converter(rudderTrimBipolarCurrent, featureBuffer, 2);
			updateFeatureBuffer();
			featureMode = 0;
		}
		saveConfig("calibrationValues.txt");
	}
	return 1;
}


//usb hid functions
void updateGearState(uint8_t* buffer, int writePosition){
	XPLMGetDatavf(pLandingGearStatus, LandingGearDeployRatio, 0, 10);

	if (LandingGearDeployRatio[0] == 1) {
		gearState = gearState | 0b00000001;
	}
	else {
		gearState = gearState & 0b11111110;
	}
	if (LandingGearDeployRatio[1] == 1) {
		gearState = gearState | 0b00000010;
	}
	else {
		gearState = gearState & 0b11111101;
	}
	if (LandingGearDeployRatio[2] == 1) {
		gearState = gearState | 0b00000100;
	}
	else {
		gearState = gearState & 0b11111011;
	}

	buffer[writePosition] = gearState;

}

void updateFlapsState(uint8_t* buffer, int writePosition){
	flapsDeployRatio = XPLMGetDataf(pFlapDeployRatio);
	flapsState = flapsDeployRatio * 255;
	buffer[writePosition] = flapsState;
}

void updateElevatorTrimState(uint8_t* buffer, int writePosition){
	autopilotMode = XPLMGetDatai(pAutopilotMode);
	elevatorTrimDeployRatio = XPLMGetDataf(pElevatorTrimDeployRatio);
	//elevatorTrimDeployRatio = -elevatorTrimDeployRatio;
	elevatorTrimState = ((elevatorTrimDeployRatio + 1) * 65535) / 2;
	buffer[writePosition] = autopilotMode;
	uint16To8Converter(elevatorTrimState, buffer, writePosition+1);
}

void updateRudderTrimState(uint8_t* buffer, int writePosition) {
	autopilotMode = XPLMGetDatai(pAutopilotMode);
	rudderTrimDeployRatio = XPLMGetDataf(pRudderTrimDeployRatio);
	//elevatorTrimDeployRatio = -elevatorTrimDeployRatio;
	rudderTrimState = ((rudderTrimDeployRatio + 1) * 65535) / 2;
	buffer[writePosition] = autopilotMode;
	uint16To8Converter(rudderTrimState, buffer, writePosition + 1);
}

void updateFeatureBuffer(void) {
	featureBuffer[1] = featureMode;
	res = hid_send_feature_report(dev, featureBuffer, FEATUREBUFFER_SIZE);
}

//other
void receive_main_monitor_bounds(int inMonitorIndex, int inLeftBx, int inTopBx, int inRightBx, int inBottomBx, void* refcon)
{
	int* main_monitor_bounds = (int*)refcon;
	if (inMonitorIndex == 0) // the main monitor
	{
		main_monitor_bounds[0] = inLeftBx;
		main_monitor_bounds[1] = inBottomBx;
		main_monitor_bounds[2] = inRightBx;
		main_monitor_bounds[3] = inTopBx;

	}
}