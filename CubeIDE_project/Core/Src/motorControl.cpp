
#include "motorControl.hpp"



void unipolarMotor::moveOneStep(){
	if(motorState == MOTOR_ENABLED){
		HAL_GPIO_WritePin(pinPorts[0], controlPins[0], unipolarStepPattern[motorPhase][0]);
		HAL_GPIO_WritePin(pinPorts[1], controlPins[1], unipolarStepPattern[motorPhase][1]);
		HAL_GPIO_WritePin(pinPorts[2], controlPins[2], unipolarStepPattern[motorPhase][2]);
		HAL_GPIO_WritePin(pinPorts[3], controlPins[3], unipolarStepPattern[motorPhase][3]);

		if(currentDir == DIR_CCW){

			if(motorPhase == LAST_PHASE){
				motorPhase = FIRST_PHASE;
			}
			else{
				motorPhase++;
			}
		}
		else if(currentDir == DIR_CW){
			if(motorPhase == FIRST_PHASE){
					motorPhase = LAST_PHASE;
				}
			else{
				motorPhase--;
			}
		}
	}
}

void unipolarMotor::moveIfRequested(){
	if(stepsToMove > 0 && motorState == MOTOR_ENABLED){
		moveOneStep();
		stepsToMove--;
	}
}

void unipolarMotor::addSteps(int steps, uint8_t direction){
	if(direction == currentDir){
		stepsToMove += steps;
	}
	else{
		stepsToMove = steps;
		currentDir = direction;
	}
}

void unipolarMotor::setSteps(int steps, uint8_t direction){
	stepsToMove = steps;
	currentDir = direction;
}

void unipolarMotor::turnOff(){
	motorState = MOTOR_DISABLED;
	HAL_GPIO_WritePin(pinPorts[0], controlPins[0], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pinPorts[1], controlPins[1], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pinPorts[2], controlPins[2], GPIO_PIN_RESET);
	HAL_GPIO_WritePin(pinPorts[3], controlPins[3], GPIO_PIN_RESET);
}

void unipolarMotor::turnOn(){
	motorState = MOTOR_ENABLED;
}

void bipolarMotor::moveOneStep(){

	if(motorState == MOTOR_ENABLED){
		if(currentDir == DIR_CW){
			HAL_GPIO_WritePin(pinPorts[1],  controlPins[1], GPIO_PIN_SET);
		}
		else if(currentDir == DIR_CCW){
			HAL_GPIO_WritePin(pinPorts[1],  controlPins[1], GPIO_PIN_RESET);
		}
		HAL_GPIO_WritePin(pinPorts[0],  controlPins[0], GPIO_PIN_SET);
		HAL_GPIO_WritePin(pinPorts[0],  controlPins[0], GPIO_PIN_RESET);
	}
}

void bipolarMotor::moveIfRequested(){

	if(stepsToMove > 0 && motorState == MOTOR_ENABLED){
		moveOneStep();
		stepsToMove--;
	}
}

void bipolarMotor::addSteps(int steps, uint8_t direction){
	if(direction == currentDir){
		stepsToMove += steps;
	}
	else{
		stepsToMove = steps;
		currentDir = direction;
	}
}

void bipolarMotor::setSteps(int steps, uint8_t direction){
	stepsToMove = steps;
	currentDir = direction;
}

void bipolarMotor::turnOff(){
	motorState = MOTOR_DISABLED;
	HAL_GPIO_WritePin(pinPorts[2],  controlPins[2], GPIO_PIN_RESET);
}

void bipolarMotor::turnOn(){
	motorState = MOTOR_ENABLED;
	HAL_GPIO_WritePin(pinPorts[2],  controlPins[2], GPIO_PIN_SET);
	HAL_GPIO_WritePin(pinPorts[3],  controlPins[3], GPIO_PIN_SET);
	int dacValue = (int)(4.095 *(float)outputCurrent);
	HAL_DAC_SetValue(&hdac1, dacChannel, DAC_ALIGN_12B_R, dacValue);
}

void bipolarMotor::setOutputCurrent(uint16_t current){
	outputCurrent = current;
	int dacValue = (int)(4.095 *(float)outputCurrent);
	HAL_DAC_SetValue(&hdac1, dacChannel, DAC_ALIGN_12B_R, dacValue);
	HAL_GPIO_TogglePin(GPIOE, GPIO_PIN_1);//test
}
