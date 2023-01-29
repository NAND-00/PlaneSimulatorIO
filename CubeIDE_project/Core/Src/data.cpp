#include<data.hpp>
#include "stm32h7xx_hal.h"
#include "stm32h7xx_hal_tim.h"

using namespace std;

#define U_STEPPER_MOTOR1   0
#define B_STEPPER_MOTOR1   1

extern ADC_HandleTypeDef hadc2;
extern ADC_HandleTypeDef hadc3;
extern DMA_HandleTypeDef hdma_adc2;
extern USBD_HandleTypeDef hUsbDeviceHS;
extern TIM_HandleTypeDef htim2;
extern bipolarMotor b_motor1;
extern unipolarMotor u_motor1;
extern bipolarMotor b_motor2;
extern unipolarMotor u_motor2;

uint8_t IN_Buffer[HID_IN_BUFFER_SIZE];
uint8_t OUT_Buffer[HID_OUT_BUFFER_SIZE];
uint8_t FEATURE_Buffer[HID_FATURE_BUFFER_SEIZE];

vector<unique_ptr<Device> > allDevices;

int readyToMoveStepper;


void ADC3_Select_CH1 (){ //PC3 MIX
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_1;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC3_Select_CH2 (){ //PF9 ETRIM
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_2;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC3_Select_CH3 (){ //PF7 BRAKE
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_3;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC3_Select_CH4 (){ //PF5 RTRIM
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_4;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC3_Select_CH5 (){ //PF3 FLAPS
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_5;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC3_Select_CH6 (){ //PF10 ROLL
	ADC_ChannelConfTypeDef sConfig = {0};
		sConfig.Channel = ADC_CHANNEL_6;
	    sConfig.Rank = ADC_REGULAR_RANK_1;
	    sConfig.SamplingTime = 4;
	    sConfig.SingleDiff = ADC_SINGLE_ENDED;
	    sConfig.OffsetNumber = ADC_OFFSET_NONE;
	    sConfig.Offset = 0;
	    sConfig.OffsetSignedSaturation = DISABLE;
	    sConfig.OffsetSign = ADC3_OFFSET_SIGN_NEGATIVE;
	    sConfig.OffsetSaturation = DISABLE;
	    if (HAL_ADC_ConfigChannel(&hadc3, &sConfig) != HAL_OK)
	    {
	      Error_Handler();
	    }
}

void ADC2_Select_CH3 (){ //PA6 LBRAKE
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_3;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC2_Select_CH5 (){ //PB1 RUDDER
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_5;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC2_Select_CH10 (){ //PC0 PITCH
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_10;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC2_Select_CH11 (){ //PC1 PROP
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_11;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC2_Select_CH14 (){ //PA2 THROTTLE
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_14;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}

void ADC2_Select_CH15 (){ //PA3 RBRAKE
	ADC_ChannelConfTypeDef sConfig = {0};
	 sConfig.Channel = ADC_CHANNEL_15;
	  sConfig.Rank = ADC_REGULAR_RANK_1;
	  sConfig.SamplingTime = 4;
	  sConfig.SingleDiff = ADC_SINGLE_ENDED;
	  sConfig.OffsetNumber = ADC_OFFSET_NONE;
	  sConfig.Offset = 0;
	  sConfig.OffsetSignedSaturation = DISABLE;
	  if (HAL_ADC_ConfigChannel(&hadc2, &sConfig) != HAL_OK)
	  {
	    Error_Handler();
	  }
}


void uint16To8Converter(uint16_t input, vector<uint8_t> &output, int writePosition){
  output[writePosition]=input & 0xff;
  output[writePosition+1]=(input >> 8);
}

void uint32To8Converter(uint32_t input, vector<uint8_t> &output, int writePosition){
  output[writePosition]=input & 0xff;
  output[writePosition+1]=(input >> 8) & 0xff;
  output[writePosition+2]=(input >> 16) & 0xff;
  output[writePosition+3]=(input >> 24) & 0xff;
}

void uint8to16Converter(uint8_t * input, uint16_t *output, int readPosition){

			*output = ((uint16_t)input[readPosition+1] << 8) | input[readPosition];
}

void activateDevices(vector<unique_ptr<Device> > &vecDevices){
  vecDevices.push_back(make_unique<Yoke>());
  vecDevices.push_back(make_unique<Pedals>());
  vecDevices.push_back(make_unique<FuelQuadrant>());
  vecDevices.push_back(make_unique<Brake>());
  vecDevices.push_back(make_unique<Switches>());
  vecDevices.push_back(make_unique<LandingGear>());
  vecDevices.push_back(make_unique<Flaps>());
  vecDevices.push_back(make_unique<ElevatorTrimWheel>());
  vecDevices.push_back(make_unique<RudderTrimWheel>());
}

int Device::returnDeviceBufferSize(){ return deviceBufferIN.size(); };

void Device::returnDeviceBuffer(uint8_t* returnArray, int writePosition){
  for (size_t i = writePosition; i < deviceBufferIN.size()+writePosition; i++) {
    returnArray[i] = deviceBufferIN[i-writePosition];
  }
}

Yoke::Yoke(): Device(4, 0){}

void Yoke::updateDeviceBuffer(){
  uint16To8Converter(x, deviceBufferIN, 0);
  uint16To8Converter(y, deviceBufferIN, 2);
  }

void Yoke::readData(){

		  ADC2_Select_CH10(); //PC0 PITCH
		  HAL_ADC_Start(&hadc2);
		  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
		  x = HAL_ADC_GetValue(&hadc2);
		  HAL_ADC_Stop(&hadc2);
		  ADC3_Select_CH6(); //PF10 ROLL
		  HAL_ADC_Start(&hadc3);
		  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
		  y = HAL_ADC_GetValue(&hadc3);
		  HAL_ADC_Stop(&hadc3);

  }

Pedals::Pedals() : Device(6, 0){}

void Pedals::updateDeviceBuffer(){
  uint16To8Converter(yaw, deviceBufferIN, 0);
  uint16To8Converter(leftBrake, deviceBufferIN, 2);
  uint16To8Converter(rightBrake, deviceBufferIN, 4);
}

void Pedals::readData(){
			ADC2_Select_CH5 (); //PB1 RUDDER
			  HAL_ADC_Start(&hadc2);
			  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
			  yaw = HAL_ADC_GetValue(&hadc2);
			  HAL_ADC_Stop(&hadc2);
			  ADC2_Select_CH3 (); //PA6 LBRAKE
			  HAL_ADC_Start(&hadc2);
			  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
			  leftBrake = HAL_ADC_GetValue(&hadc2);
			  HAL_ADC_Stop(&hadc2);
			  ADC2_Select_CH15 (); //PA3 RBRAKE
			  HAL_ADC_Start(&hadc2);
			  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
			  rightBrake = HAL_ADC_GetValue(&hadc2);
			  HAL_ADC_Stop(&hadc2);
}

FuelQuadrant::FuelQuadrant() : Device(6, 0){}

void FuelQuadrant::updateDeviceBuffer(){
	  uint16To8Converter(throttle, deviceBufferIN, 0);
	  uint16To8Converter(propeller, deviceBufferIN, 2);
	  uint16To8Converter(mixture, deviceBufferIN, 4);
	}

void FuelQuadrant::readData(){
	ADC2_Select_CH14 (); //PA2 THROTTLE
	  HAL_ADC_Start(&hadc2);
	  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	  throttle = HAL_ADC_GetValue(&hadc2);
	  HAL_ADC_Stop(&hadc2);
	  ADC2_Select_CH11 (); //PC1 PROP
	  HAL_ADC_Start(&hadc2);
	  HAL_ADC_PollForConversion(&hadc2, HAL_MAX_DELAY);
	  propeller = HAL_ADC_GetValue(&hadc2);
	  HAL_ADC_Stop(&hadc2);
	  ADC3_Select_CH1 (); //PC3 MIX
	  HAL_ADC_Start(&hadc3);
	  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	  mixture = HAL_ADC_GetValue(&hadc3);
	  HAL_ADC_Stop(&hadc3);
}

Brake::Brake() : Device(2, 0){}

void Brake::updateDeviceBuffer(){
	  uint16To8Converter(brake, deviceBufferIN, 0);
}

void Brake::readData(){
	ADC3_Select_CH3 (); //PF7 BRAKE
	  HAL_ADC_Start(&hadc3);
	  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	  brake = HAL_ADC_GetValue(&hadc3);
	  HAL_ADC_Stop(&hadc3);

}

Switches::Switches() : Device(4, 0){}

void Switches::updateDeviceBuffer(){
	deviceBufferIN[0] = switches1;
	deviceBufferIN[1] = switches2;
	deviceBufferIN[2] = switches3;
	deviceBufferIN[3] = switches4;
}

void Switches::readData(){
	int input;
	switches1 = 0;
	switches2 = 0;
	switches3 = 0;
	switches4 = 0;

	HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_RESET);//CS1
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_RESET); // enable parallel input
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);//clock low
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);//clock high -> data loaded
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_1, GPIO_PIN_SET);//enable serial out
	 for(int i = 0; i < 8; i++){
		 input = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15); //read each bit
		 if(input){
			 switches1 = switches1 | (1 << i);
		 }
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); //clock low
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); //clock high
	 }
	 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_10, GPIO_PIN_SET);//CS1

	 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_RESET);//CS2
	 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_RESET); // enable parallel input
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);//clock low
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);//clock high -> data loaded
		 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_0, GPIO_PIN_SET);//enable serial out
		 switches2 = 0;
		 for(int i = 0; i < 8; i++){
			 input = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15); //read each bit
			 if(input){
				 switches2 = switches2 | (1 << i);
			 }
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); //clock low
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); //clock high
		 }
		 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_11, GPIO_PIN_SET);//CS2

		 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_RESET);//CS3
	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET); // enable parallel input
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);//clock low
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);//clock high -> data loaded
		 HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);//enable serial out
		 switches3 = 0;
		 for(int i = 0; i < 8; i++){
			 input = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15); //read each bit
			 if(input){
				 switches3 = switches3 | (1 << i);
			 }
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); //clock low
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); //clock high
		 }
		 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);//CS3

		 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_13, GPIO_PIN_RESET);//CS4
	 	 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET); // enable parallel input
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET);//clock low
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET);//clock high -> data loaded
		 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_SET);//enable serial out
		 for(int i = 0; i < 8; i++){
			 input = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_15); //read each bit
			 if(input){
				 switches4 = switches4 | (1 << i);
			 }
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_RESET); //clock low
			 HAL_GPIO_WritePin(GPIOC, GPIO_PIN_11, GPIO_PIN_SET); //clock high
		 }
		 HAL_GPIO_WritePin(GPIOG, GPIO_PIN_12, GPIO_PIN_SET);//CS3

}

LandingGear::LandingGear() : Device(0, 1){}

void LandingGear::updateDeviceState(){
	if(gearState & 0b00000001){
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
	}
	if((gearState>>1) & 0b00000001){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_5, GPIO_PIN_RESET);
	}
	if((gearState>>2) & 0b00000001){
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_SET);
	}
	else{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_4, GPIO_PIN_RESET);
	}
}

void LandingGear::readData(){

gearState = OUT_Buffer[0];

}

Flaps::Flaps() : Device(2, 1){
	servoMinValue = 750;
	servoRange = 3000;
}

void Flaps::readData(){
	flapsServoPos = OUT_Buffer[1];
	if(FEATURE_Buffer[0] == 1){
			uint8to16Converter(FEATURE_Buffer, &servoMinValue, 1);
			uint8to16Converter(FEATURE_Buffer, &servoRange, 3);
			FEATURE_Buffer[0] = 0;
		}
	ADC3_Select_CH5 (); //PF3 FLAPS
	  HAL_ADC_Start(&hadc3);
	  HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
	  flapsHandle = HAL_ADC_GetValue(&hadc3);
	  HAL_ADC_Stop(&hadc3);
}

void Flaps::updateDeviceBuffer(){
	uint16To8Converter(flapsHandle, deviceBufferIN, 0);


}

void Flaps::updateDeviceState(){

	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_4, (int)((servoRange*flapsServoPos)/255)+servoMinValue);
}

ElevatorTrimWheel::ElevatorTrimWheel() : Device(2, 1){
	servoMinValue = 750;
	servoRange = 3000;
	rotaryLastState = 0;
	elevatorTrim = 0;
	wheelPositionChangeCW = 0;
	wheelPositionChangeCCW = 0;
	autopilotMode = 0;
	trimServoPos = 128;
	wheelInputType = 1;
	stepperType = 1;
	wheelRange = 6;
	uStepperResolution = 2038;
	bStepperResolution = 400;
	uniStepperMotorStepSize = 65535/wheelRange/uStepperResolution;
	biStepperMotorStepSize = 65535/wheelRange/bStepperResolution;
	AXISPositionChange = 0;
	rotaryResolution = 64;
	bStepperCurrent = 250;
}

void ElevatorTrimWheel::readData(){
	//check for data outMode
	//calibration of servo - mode 2
	if(FEATURE_Buffer[0] == 2){
		uint8to16Converter(FEATURE_Buffer, &servoMinValue, 1);
		uint8to16Converter(FEATURE_Buffer, &servoRange, 3);
		FEATURE_Buffer[0] = 0;
	}
	//set input type - mode 4
	else if(FEATURE_Buffer[0] == 4){
		wheelInputType = FEATURE_Buffer[2];
		FEATURE_Buffer[0] = 0;
	}
	//set stepper type - mode 5
	else if(FEATURE_Buffer[0] == 5){
		stepperType =  FEATURE_Buffer[2];
		if(stepperType == 0){
			b_motor1.turnOff();
			u_motor1.turnOff();
		}
		else if(stepperType == 1){
			b_motor1.turnOff();
		}
		else if(stepperType == 2){
			//b_motor1.turnOn();
			u_motor1.turnOff();
		}
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 6){
		rotaryResolution = FEATURE_Buffer[2];
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 7){
		uint8to16Converter(FEATURE_Buffer, &uStepperResolution, 1);
		uniStepperMotorStepSize = 65535/wheelRange/uStepperResolution;
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 8){
		uint8to16Converter(FEATURE_Buffer, &bStepperResolution, 1);
		biStepperMotorStepSize = 65535/wheelRange/bStepperResolution;
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 9){
		uint8to16Converter(FEATURE_Buffer, &bStepperCurrent, 1);
		b_motor1.setOutputCurrent(bStepperCurrent);
		FEATURE_Buffer[0] = 0;
	}

	//check for autopilot state
	autopilotMode = OUT_Buffer[2];
	uint16_t lastTrimDeployPos = trimDeployPos;//last position of trim for stepper movement calculation in autopilot mode
	uint8to16Converter(OUT_Buffer, &trimDeployPos, 3);//current position of trim for servo indicator and stepper in autopilot mode

	if(autopilotMode == 0 && wheelInputType == 1){	//if autopilot is disabled  && input is encoder, read position of the wheel

	uint8_t A = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_2);
	uint8_t B = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_3);
	uint8_t input = A | (B<<1);
		if(input != rotaryLastState){
			switch(input){
			case 0:
				if(rotaryLastState == 2){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 1:
				if(rotaryLastState == 0){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 3:
				if(rotaryLastState == 1){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 2:
				if(rotaryLastState == 3){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;

				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			default:
				break;
			}
		}
		rotaryLastState = input;
		AXISPositionChange = 0;
	}
	else if (autopilotMode != 0 && wheelInputType == 1){//if autopilot enabled && rotary input
			elevatorTrim = (((trimDeployPos >> 4)&4095));//the trim position is defined by trim position in simulator (when autopilot will be disabled, the virtual axis of device and simulator will be synchronized)
			AXISPositionChange += (int)(trimDeployPos - lastTrimDeployPos);//how much did the axis change (for stepper movement)
		}

	if(wheelInputType == 0){ //if input type is pot
		ADC3_Select_CH2 (); //PF9 ETRIM
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
		elevatorTrim = HAL_ADC_GetValue(&hadc3);
		HAL_ADC_Stop(&hadc3);
		if(autopilotMode != 0){
			AXISPositionChange = (int)(trimDeployPos - (elevatorTrim << 4));
		}
		else{
			AXISPositionChange = 0;
		}
	}

	trimServoPos = (((trimDeployPos>>8)&255));
}

void ElevatorTrimWheel::updateDeviceBuffer(){
	if(autopilotMode == 0){ //if autopilot disabled, write position of physical wheel to simulator
		uint16To8Converter(elevatorTrim, deviceBufferIN, 0);
	}
}

void ElevatorTrimWheel::updateDeviceState(){
	if(autopilotMode != 0){ //if autopilot is enabled, move stepper to current wheel position

		if(stepperType == 2){
			b_motor1.turnOn();
		}
		else if(stepperType == 1){
			u_motor1.turnOn();
		}
		//if motor is bipolar && rotary encoder input
		if(stepperType == 2 && wheelInputType == 1){
			//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > biStepperMotorStepSize){
				wheelPositionChangeCW = AXISPositionChange/biStepperMotorStepSize;
				AXISPositionChange -= wheelPositionChangeCW*biStepperMotorStepSize;
				b_motor1.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if (AXISPositionChange < (int)(0 - biStepperMotorStepSize)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/biStepperMotorStepSize;
				AXISPositionChange += wheelPositionChangeCCW*biStepperMotorStepSize;
				b_motor1.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
		}
		//if motor is unipolar && rotary encoder input
		else if(stepperType == 1 && wheelInputType == 1){
		//precise movement mode
			//calculate number of steps and direction for unipolar stepper
			if(AXISPositionChange > biStepperMotorStepSize){
				wheelPositionChangeCW = AXISPositionChange/uniStepperMotorStepSize;
				AXISPositionChange -= wheelPositionChangeCW*uniStepperMotorStepSize;
				u_motor1.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if (AXISPositionChange < (int)(0 - biStepperMotorStepSize)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/uniStepperMotorStepSize;//biStepperMotorStepSize;
				AXISPositionChange += wheelPositionChangeCCW*uniStepperMotorStepSize;
				u_motor1.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
		}
		//if motor is bipolar && pot input
		else if(stepperType == 2 && wheelInputType == 0){
		//precise position mode
		//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > (65535/100)){
				wheelPositionChangeCW = AXISPositionChange/biStepperMotorStepSize;
				b_motor1.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if(AXISPositionChange < (-65535/100)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/biStepperMotorStepSize;
				b_motor1.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
			else if(elevatorTrim > (4095 - (4095/100)) || elevatorTrim < (4095/100) || abs(AXISPositionChange) < (65535/100)){
				b_motor1.setSteps(0, DIR_CW);
				b_motor1.turnOff();
			}
				AXISPositionChange = 0;
		}
		//if motor is unipolar && pot input
		else if(stepperType == 1 && wheelInputType == 0){
		//precise position mode
		//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > (65535/100)){
				wheelPositionChangeCW = AXISPositionChange/uniStepperMotorStepSize;
				u_motor1.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if(AXISPositionChange < (-65535/100)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/uniStepperMotorStepSize;
				u_motor1.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
			else if(elevatorTrim > (4095 - (4095/100)) || elevatorTrim < (4095/100) || abs(AXISPositionChange) < (65535/100)){
				u_motor1.setSteps(0, DIR_CW);
				u_motor1.turnOff();
			}
				AXISPositionChange = 0;
		}
	}
	else{
		b_motor1.setSteps(0, DIR_CW);
		b_motor1.turnOff();
		u_motor1.setSteps(0, DIR_CW);
		u_motor1.turnOff();
		}

	wheelPositionChangeCW = 0;
	wheelPositionChangeCCW = 0;


	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_3, (int)((servoRange*trimServoPos)/255)+servoMinValue);
}



RudderTrimWheel::RudderTrimWheel() : Device(2, 1){
	servoMinValue = 750;
	servoRange = 3000;
	rotaryLastState = 0;
	elevatorTrim = 0;
	wheelPositionChangeCW = 0;
	wheelPositionChangeCCW = 0;
	autopilotMode = 0;
	trimServoPos = 128;
	wheelInputType = 1;
	stepperType = 1;
	wheelRange = 6;
	uStepperResolution = 2038;
	bStepperResolution = 400;
	uniStepperMotorStepSize = 65535/wheelRange/uStepperResolution;
	biStepperMotorStepSize = 65535/wheelRange/bStepperResolution;
	AXISPositionChange = 0;
	rotaryResolution = 64;
	bStepperCurrent = 250;
}

void RudderTrimWheel::readData(){
	//check for data outMode
	//calibration of servo - mode 3
	if(FEATURE_Buffer[0] == 3){
		uint8to16Converter(FEATURE_Buffer, &servoMinValue, 1);
		uint8to16Converter(FEATURE_Buffer, &servoRange, 3);
		FEATURE_Buffer[0] = 0;
	}
	//set input type - mode 10
	else if(FEATURE_Buffer[0] == 10){
		wheelInputType = FEATURE_Buffer[2];
		FEATURE_Buffer[0] = 0;
	}
	//set stepper type - mode 11
	else if(FEATURE_Buffer[0] == 11){
		stepperType =  FEATURE_Buffer[2];
		if(stepperType == 0){
			b_motor2.turnOff();
			u_motor2.turnOff();
		}
		else if(stepperType == 1){
			b_motor2.turnOff();
		}
		else if(stepperType == 2){
			u_motor1.turnOff();
		}
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 12){
		rotaryResolution = FEATURE_Buffer[2];
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 13){
		uint8to16Converter(FEATURE_Buffer, &uStepperResolution, 1);
		uniStepperMotorStepSize = 65535/wheelRange/uStepperResolution;
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 14){
		uint8to16Converter(FEATURE_Buffer, &bStepperResolution, 1);
		biStepperMotorStepSize = 65535/wheelRange/bStepperResolution;
		FEATURE_Buffer[0] = 0;
	}
	else if(FEATURE_Buffer[0] == 15){
		uint8to16Converter(FEATURE_Buffer, &bStepperCurrent, 1);
		b_motor2.setOutputCurrent(bStepperCurrent);
		FEATURE_Buffer[0] = 0;
	}

	//check for autopilot state
	autopilotMode = OUT_Buffer[5];


	uint16_t lastTrimDeployPos = trimDeployPos;//last position of trim for stepper movement calculation in autopilot mode
	uint8to16Converter(OUT_Buffer, &trimDeployPos, 6);//current position of trim for servo indicator and stepper in autopilot mode

	if(autopilotMode == 0 && wheelInputType == 1){	//if autopilot is disabled  && input is encoder, read position of the wheel

	uint8_t A = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_5);
	uint8_t B = HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_4);
	uint8_t input = A | (B<<1);
		if(input != rotaryLastState){
			switch(input){
			case 0:
				if(rotaryLastState == 2){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 1:
				if(rotaryLastState == 0){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 3:
				if(rotaryLastState == 1){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;
				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			case 2:
				if(rotaryLastState == 3){
					elevatorTrim = (elevatorTrim >= 4095-683/rotaryResolution) ?  4095 : elevatorTrim+683/rotaryResolution;

				}
				else{
					elevatorTrim = (elevatorTrim <= 683/rotaryResolution) ?  0 : elevatorTrim-683/rotaryResolution;
				}
				break;
			default:
				break;
			}
		}
		rotaryLastState = input;
		AXISPositionChange = 0;
	}
	else if (autopilotMode != 0 && wheelInputType == 1){//if autopilot enabled && rotary input
			elevatorTrim = (((trimDeployPos >> 4)&4095));//the trim position is defined by trim position in simulator (when autopilot will be disabled, the virtual axis of device and simulator will be synchronized)
			AXISPositionChange += (int)(trimDeployPos - lastTrimDeployPos);//how much did the axis change (for stepper movement)
		}

	if(wheelInputType == 0){ //if input type is pot
		ADC3_Select_CH4 (); //PF5 RTRIM
		HAL_ADC_Start(&hadc3);
		HAL_ADC_PollForConversion(&hadc3, HAL_MAX_DELAY);
		elevatorTrim = HAL_ADC_GetValue(&hadc3);
		HAL_ADC_Stop(&hadc3);
		if(autopilotMode != 0){
			AXISPositionChange = (int)(trimDeployPos - (elevatorTrim << 4));
		}
		else{
			AXISPositionChange = 0;
		}
	}

	trimServoPos = (255 - ((trimDeployPos>>8)&255));
}

void RudderTrimWheel::updateDeviceBuffer(){
	if(autopilotMode == 0){ //if autopilot disabled, write position of physical wheel to simulator
		uint16To8Converter(elevatorTrim, deviceBufferIN, 0);
	}
}

void RudderTrimWheel::updateDeviceState(){
	if(autopilotMode != 0){ //if autopilot is enabled, move stepper to current wheel position

		if(stepperType == 2){
			b_motor2.turnOn();
		}
		else if(stepperType == 1){
			u_motor2.turnOn();
		}
		//if motor is bipolar && rotary encoder input
		if(stepperType == 2 && wheelInputType == 1){
			//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > biStepperMotorStepSize){
				wheelPositionChangeCW = AXISPositionChange/biStepperMotorStepSize;
				AXISPositionChange -= wheelPositionChangeCW*biStepperMotorStepSize;
				b_motor2.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if (AXISPositionChange < (int)(0 - biStepperMotorStepSize)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/biStepperMotorStepSize;
				AXISPositionChange += wheelPositionChangeCCW*biStepperMotorStepSize;
				b_motor2.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
		}
		//if motor is unipolar && rotary encoder input
		else if(stepperType == 1 && wheelInputType == 1){
		//precise movement mode
			//calculate number of steps and direction for unipolar stepper
			if(AXISPositionChange > biStepperMotorStepSize){
				wheelPositionChangeCW = AXISPositionChange/uniStepperMotorStepSize;
				AXISPositionChange -= wheelPositionChangeCW*uniStepperMotorStepSize;
				u_motor2.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if (AXISPositionChange < (int)(0 - biStepperMotorStepSize)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/uniStepperMotorStepSize;//biStepperMotorStepSize;
				AXISPositionChange += wheelPositionChangeCCW*uniStepperMotorStepSize;
				u_motor2.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
		}
		//if motor is bipolar && pot input
		else if(stepperType == 2 && wheelInputType == 0){
		//precise position mode
		//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > (65535/100)){
				wheelPositionChangeCW = AXISPositionChange/biStepperMotorStepSize;
				b_motor2.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if(AXISPositionChange < (-65535/100)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/biStepperMotorStepSize;
				b_motor2.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
			else if(elevatorTrim > (4095 - (4095/100)) || elevatorTrim < (4095/100) || abs(AXISPositionChange) < (65535/100)){
				b_motor2.setSteps(0, DIR_CW);
				b_motor2.turnOff();
			}
				AXISPositionChange = 0;
		}
		//if motor is unipolar && pot input
		else if(stepperType == 1 && wheelInputType == 0){
		//precise position mode
		//calculate number of steps and direction for bipolar stepper
			if(AXISPositionChange > (65535/100)){
				wheelPositionChangeCW = AXISPositionChange/uniStepperMotorStepSize;
				u_motor2.addSteps(wheelPositionChangeCW, DIR_CW);
			}
			else if(AXISPositionChange < (-65535/100)){
				wheelPositionChangeCCW = abs(AXISPositionChange)/uniStepperMotorStepSize;
				u_motor2.addSteps(wheelPositionChangeCCW, DIR_CCW);
			}
			else if(elevatorTrim > (4095 - (4095/100)) || elevatorTrim < (4095/100) || abs(AXISPositionChange) < (65535/100)){
				u_motor2.setSteps(0, DIR_CW);
				u_motor2.turnOff();
			}
				AXISPositionChange = 0;
		}
	}
	else{
		b_motor2.setSteps(0, DIR_CW);
		b_motor2.turnOff();
		u_motor2.setSteps(0, DIR_CW);
		u_motor2.turnOff();
		}

	wheelPositionChangeCW = 0;
	wheelPositionChangeCCW = 0;


	__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, (int)((servoRange*trimServoPos)/255)+servoMinValue);
}
