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
#include "utils.hpp"

#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif

#define MAX_STR 255
#define OUTBUFFER_SIZE 9
#define FEATUREBUFFER_SIZE 6

/*Dataref variables*/
extern XPLMDataRef pLandingGearStatus;
extern XPLMDataRef pFlapDeployRatio;
extern XPLMDataRef pElevatorTrimDeployRatio;
extern XPLMDataRef pAutopilotMode;
extern XPLMDataRef pRudderTrimDeployRatio;
extern float LandingGearDeployRatio[10];
extern float flapsDeployRatio;
extern float elevatorTrimDeployRatio;
extern float rudderTrimDeployRatio;
extern int autopilotMode;


/*OUT data variables*/
extern uint8_t gearState;
extern int   DeployStatNose;
extern int   DeployStatLeft;
extern int   DeployStatRight;
extern uint8_t flapsState;
extern uint16_t elevatorTrimState;
extern uint16_t rudderTrimState;
extern uint8_t autopilotState;

/*feature status variables*/
extern uint8_t featureMode; //is the value of featureBuffer[1] and defines the mode of feature status values featureBuffer[2-5]
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
extern uint8_t outBuffer[OUTBUFFER_SIZE];
extern uint8_t featureBuffer[FEATUREBUFFER_SIZE];
extern uint8_t outReportID; //outBuffer[0] value
extern uint8_t featureReportID; //featureBuffer[0] value
extern uint16_t flapsServoRange;
extern uint16_t flapsServoMinValue;
extern uint16_t elevatorTrimServoRange;
extern uint16_t elevatorTrimServoMinValue;
extern uint16_t rudderTrimServoRange;
extern uint16_t rudderTrimServoMinValue;
extern int elevatorTrimInputType; //0 - potentiometer, 1 - rotary encoder
extern int elevatorTrimActuatorType; //0 - none, 1 - unipolar stepper, 2 bipolar stepper
extern int elevatorTrimEncoderResolution;
extern uint16_t elevatorTrimUnipolarResolution;
extern uint16_t elevatorTrimBipolarResolution;
extern uint16_t elevatorTrimBipolarCurrent;

extern int rudderTrimInputType; //0 - potentiometer, 1 - rotary encoder
extern int rudderTrimActuatorType; //0 - none, 1 - unipolar stepper, 2 bipolar stepper
extern int rudderTrimEncoderResolution;
extern uint16_t rudderTrimUnipolarResolution;
extern uint16_t rudderTrimBipolarResolution;
extern uint16_t rudderTrimBipolarCurrent;

/*Plugin status variables*/
extern int servoCalibratingStatus[3];

/*hid variables*/
extern hid_device* dev;
extern int res;
extern unsigned char buf[65];
extern uint16_t buf16_t[65];
extern int i;
extern int vid; // ; 0x765
extern int pid; //; 0x5020