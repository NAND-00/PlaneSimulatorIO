#include "utils.hpp"
#include "globalVariables.hpp"
#include "hidCommunication.hpp"

//output file values
char	outputPath[255];
static FILE* gOutputFile;

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

void uint16To8Converter(uint16_t input, uint8_t* output, int writePosition) {
	output[writePosition] = input & 0xff;
	output[writePosition + 1] = (input >> 8);
}

int	coord_in_rect(float x, float y, float* bounds_lbrt) {
	return ((x >= bounds_lbrt[0]) && (x < bounds_lbrt[2]) && (y < bounds_lbrt[3]) && (y >= bounds_lbrt[1])); 
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

void initConfig() {
	XPLMGetSystemPath(outputPath);
	strcat(outputPath, "calibrationValues.txt");
	gOutputFile = fopen(outputPath, "r");
	if (!gOutputFile) {
		saveConfig("calibrationValues.txt");
	}
	else {
		readConfig("calibrationValues.txt");
	}
}