#include "usbd_hid.h"
#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <stdint.h>
#include <memory>
#include <bitset>
#include <any>
#include "motorControl.hpp"

#ifndef INC_DATA_H_
#define INC_DATA_H_
#endif /* INC_DATA_H_ */

#define HID_IN_BUFFER_SIZE 28
#define HID_OUT_BUFFER_SIZE 8
#define HID_FATURE_BUFFER_SEIZE 5

using namespace std;

typedef uint16_t AXIS;



class Device{
protected:
	std::vector<uint8_t> deviceBufferIN;
	std::vector<uint8_t> deviceBufferOUT;
public:
  	int returnDeviceBufferSize();
	void returnDeviceBuffer(uint8_t* inputArray, int writePosition);
	virtual void readData(){};
	virtual void updateDeviceState(){};
	virtual void updateDeviceBuffer(){};

  Device(int BufferSizeIN, int BufferSizeOUT) : deviceBufferIN(BufferSizeIN), deviceBufferOUT(BufferSizeOUT){};
};

class Yoke : public Device{
protected:
	volatile AXIS x;
	volatile AXIS y;
public:
	Yoke();
	void updateDeviceBuffer();
	void readData();
};

class Pedals : public Device{
	volatile AXIS yaw;
	volatile AXIS leftBrake;
	volatile AXIS rightBrake;
public:
	Pedals();
	void updateDeviceBuffer();
	void readData();
};

class FuelQuadrant : public Device{
	volatile AXIS throttle;
	volatile AXIS propeller;
	volatile AXIS mixture;
public:
	FuelQuadrant();
	void updateDeviceBuffer();
	void readData();
};

class Brake : public Device {
	AXIS brake;
public:
	Brake();
	void updateDeviceBuffer();
	void readData();
};

class Switches : public Device {
	uint8_t switches1;
	uint8_t switches2;
	uint8_t switches3;
	uint8_t switches4;
public:
	Switches();
	void updateDeviceBuffer();
	void readData();
};

class LandingGear : public Device {
	uint8_t gearState;
public:
	LandingGear();
	void updateDeviceState();
	void readData();
};

class Flaps : public Device{
	uint8_t flapsServoPos;
	uint16_t servoMinValue;
	uint16_t servoRange;
	AXIS flapsHandle;
public:
	Flaps();
	void updateDeviceState();
	void readData();
	void updateDeviceBuffer();
};

class ElevatorTrimWheel : public Device{
	uint16_t trimDeployPos;

	uint8_t trimServoPos;
	uint16_t servoMinValue;
	uint16_t servoRange;
	uint8_t rotaryLastState;
	uint8_t rotaryResolution;

	int AXISPositionChange; //change of trim axis compared to last data
	uint16_t wheelPositionChangeCW;
	uint16_t wheelPositionChangeCCW;
	uint16_t uniStepperMotorStepSize; //how big axis change is equivalent to one unipolar stepper step
	uint16_t biStepperMotorStepSize;//how big axis change is equivalent to one bipolar stepper step
	uint8_t wheelRange; //number of full wheel turns
	uint8_t stepperType; // 1 - unipolar, 2 - bipolar, 0 -none
	uint16_t uStepperResolution;
	uint16_t bStepperResolution;
	uint16_t bStepperCurrent;

	uint8_t autopilotMode; //0-autopilot off, 1 - on

	AXIS elevatorTrim; //0-4095 position of trim axis
	uint8_t wheelInputType; //0 - potentiometer, 1 - encoder
public:
	ElevatorTrimWheel();
	void updateDeviceState();
	void readData();
	void updateDeviceBuffer();
};

class RudderTrimWheel : public Device{
	uint16_t trimDeployPos;

	uint8_t trimServoPos;
	uint16_t servoMinValue;
	uint16_t servoRange;
	uint8_t rotaryLastState;
	uint8_t rotaryResolution;

	int AXISPositionChange; //change of trim axis compared to last data
	uint16_t wheelPositionChangeCW;
	uint16_t wheelPositionChangeCCW;
	uint16_t uniStepperMotorStepSize; //how big axis change is equivalent to one unipolar stepper step
	uint16_t biStepperMotorStepSize;//how big axis change is equivalent to one bipolar stepper step
	uint8_t wheelRange; //number of full wheel turns
	uint8_t stepperType; // 1 - unipolar, 2 - bipolar, 0 -none
	uint16_t uStepperResolution;
	uint16_t bStepperResolution;
	uint16_t bStepperCurrent;

	uint8_t autopilotMode; //0-autopilot off, 1 - on

	AXIS elevatorTrim; //0-4095 position of trim axis
	uint8_t wheelInputType; //0 - potentiometer, 1 - encoder
public:
	RudderTrimWheel();
	void updateDeviceState();
	void readData();
	void updateDeviceBuffer();
};


void activateDevices(vector<unique_ptr<Device> > &vecDevices);

void uint16To8Converter(uint16_t input, vector<uint8_t> &output, int writePosition);


