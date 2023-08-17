#include "hidCommunication.hpp"
#include "globalVariables.hpp"

void updateGearState(uint8_t* buffer, int writePosition) {
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

void updateFlapsState(uint8_t* buffer, int writePosition) {
	flapsDeployRatio = XPLMGetDataf(pFlapDeployRatio);
	flapsState = flapsDeployRatio * 255;
	buffer[writePosition] = flapsState;
}

void updateElevatorTrimState(uint8_t* buffer, int writePosition) {
	autopilotMode = XPLMGetDatai(pAutopilotMode);
	elevatorTrimDeployRatio = XPLMGetDataf(pElevatorTrimDeployRatio);
	//elevatorTrimDeployRatio = -elevatorTrimDeployRatio;
	elevatorTrimState = ((elevatorTrimDeployRatio + 1) * 65535) / 2;
	buffer[writePosition] = autopilotMode;
	uint16To8Converter(elevatorTrimState, buffer, writePosition + 1);
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
