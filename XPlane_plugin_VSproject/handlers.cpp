#include "handlers.hpp"
#include "globalVariables.hpp"
#include "graphics.hpp"
#include "hidCommunication.hpp"
#include "utils.hpp"

//dummy handlers
int					dummy_mouse_handler(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon) { return 0; }
XPLMCursorStatus	dummy_cursor_status_handler(XPLMWindowID in_window_id, int x, int y, void* in_refcon) { return xplm_CursorDefault; }
int					dummy_wheel_handler(XPLMWindowID in_window_id, int x, int y, int wheel, int clicks, void* in_refcon) { return 0; }
void				dummy_key_handler(XPLMWindowID in_window_id, char key, XPLMKeyFlags flags, char virtual_key, void* in_refcon, int losing_focus) { }

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
