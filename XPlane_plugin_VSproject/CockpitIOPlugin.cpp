
#include "globalVariables.hpp"
#include "handlers.hpp"
#include "graphics.hpp"
#include "hidCommunication.hpp"

//hid test ***




//hid_device* handle;
//wchar_t wstr[MAX_STR];
//const wchar_t* out = NULL;



// Callbacks we will register when we create our window




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
	closeAllWindows();
}

PLUGIN_API void XPluginDisable(void) { }
PLUGIN_API int  XPluginEnable(void)  { return 1; }
PLUGIN_API void XPluginReceiveMessage(XPLMPluginID inFrom, int inMsg, void * inParam) { }


//usb hid functions

