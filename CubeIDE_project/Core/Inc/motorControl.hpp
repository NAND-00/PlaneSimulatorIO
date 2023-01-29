#include "stm32h7xx.h"
#include <stdint.h>

#define DIR_CW 1
#define DIR_CCW 0
#define MOTOR_DISABLED 0
#define MOTOR_ENABLED 1
#define FIRST_PHASE 0
#define LAST_PHASE 3

extern DAC_HandleTypeDef hdac1;

static GPIO_PinState unipolarStepPattern[4][4] = {
		{GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET},
		{GPIO_PIN_RESET, GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET},
		{GPIO_PIN_RESET, GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET},
		{GPIO_PIN_SET, GPIO_PIN_SET, GPIO_PIN_RESET, GPIO_PIN_RESET}
};

class unipolarMotor{
protected:
	GPIO_TypeDef * pinPorts[4];
	uint16_t controlPins[4];

	uint8_t currentDir;
	uint8_t stepsToMove;
	uint8_t motorState;
	uint8_t motorPhase;
public:
	void moveOneStep();
	void moveIfRequested();
	void addSteps(int steps, uint8_t direction);
	void setSteps(int steps, uint8_t direction);
	void turnOff();
	void turnOn();

unipolarMotor(GPIO_TypeDef * firstPort, uint16_t firstPin, GPIO_TypeDef * secondPort, uint16_t secondPin, GPIO_TypeDef * thirdPort, uint16_t thirdPin, GPIO_TypeDef * fourthPort, uint16_t fourthPin){
		pinPorts[0] = firstPort;
		pinPorts[1] = secondPort;
		pinPorts[2] = thirdPort;
		pinPorts[3] = fourthPort;

		controlPins[0] = firstPin;
		controlPins[1] = secondPin;
		controlPins[2] = thirdPin;
		controlPins[3] = fourthPin;

		motorState = MOTOR_DISABLED;
		motorPhase = FIRST_PHASE;
		currentDir = DIR_CW;
		stepsToMove = 0;
	};
};

class bipolarMotor{
protected:
	GPIO_TypeDef * pinPorts[6];
	uint16_t controlPins[6];
	uint8_t dacChannel;

	uint8_t currentDir;
	uint8_t stepsToMove;
	uint8_t motorState;
	uint16_t outputCurrent;
public:
	void moveOneStep();
	void moveIfRequested();
	void addSteps(int steps, uint8_t direction);
	void setSteps(int steps, uint8_t direction);
	void turnOff();
	void turnOn();
	void setOutputCurrent(uint16_t current);
	bipolarMotor(GPIO_TypeDef * firstPort, uint16_t firstPin, GPIO_TypeDef * secondPort, uint16_t secondPin, GPIO_TypeDef * thirdPort, uint16_t thirdPin, GPIO_TypeDef * fourthPort, uint16_t fourthPin, GPIO_TypeDef * fifthPort, uint16_t fifthPin, GPIO_TypeDef * sixthPort, uint16_t sixthPin, uint8_t dacCh){
		pinPorts[0] = firstPort;
		pinPorts[1] = secondPort;
		pinPorts[2] = thirdPort;
		pinPorts[3] = fourthPort;
		pinPorts[4] = fifthPort;
		pinPorts[5] = sixthPort;

		controlPins[0] = firstPin; //step
		controlPins[1] = secondPin; //dir
		controlPins[2] = thirdPin; //enable
		controlPins[3] = fourthPin; //reset
		controlPins[4] = fifthPin; //mode0
		controlPins[5] = sixthPin; //mode1

		dacChannel = dacCh;
		motorState = MOTOR_DISABLED;
		currentDir = DIR_CW;
		stepsToMove = 0;
		outputCurrent = 500;

		HAL_GPIO_WritePin(pinPorts[2],  controlPins[2], GPIO_PIN_RESET);
		HAL_GPIO_WritePin(pinPorts[3],  controlPins[3], GPIO_PIN_SET);
	}
};

