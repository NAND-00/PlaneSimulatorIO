#include "globalVariables.hpp"

#ifndef XPLM300
#error This is made to be compiled against the XPLM300 SDK
#endif

#define MAX_STR 255
#define OUTBUFFER_SIZE 9
#define FEATUREBUFFER_SIZE 6

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