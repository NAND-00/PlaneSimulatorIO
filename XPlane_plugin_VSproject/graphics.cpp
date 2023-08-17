#include "graphics.hpp"
#include "globalVariables.hpp"

//draw functions
void drawButton(float* button_lbrt) {
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glColor4fv(white);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(button_lbrt[0], button_lbrt[3]);
		glVertex2i(button_lbrt[2], button_lbrt[3]);
		glVertex2i(button_lbrt[2], button_lbrt[1]);
		glVertex2i(button_lbrt[0], button_lbrt[1]);
	}
	glEnd();
}

void drawCalibrateServo(XPLMWindowID in_window_id, void* in_refcon)
{


	float col_white[] = { 1.0, 1.0, 1.0 };
	XPLMSetGraphicsState(
		0 /* no fog */,
		0 /* 0 texture units */,
		0 /* no lighting */,
		0 /* no alpha testing */,
		1 /* do alpha blend */,
		1 /* do depth testing */,
		0 /* no depth writing */
	);
	// We draw our rudimentary button boxes based on the height of the button text
	int char_height;
	XPLMGetFontDimensions(xplmFont_Proportional, NULL, &char_height, NULL);

	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

	// Draw our buttons
	// Position the move up/move down button in the upper left of the window 
	servo_move_down_lbrt[0] = l + 10;
	servo_move_down_lbrt[3] = t - 15;
	servo_move_down_lbrt[2] = servo_move_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, move_down_label, strlen(move_down_label)) + 8; // *just* wide enough to fit the button text
	servo_move_down_lbrt[1] = servo_move_down_lbrt[3] - (1.25f * char_height); // a bit taller than the button text


	servo_move_up_lbrt[0] = servo_move_down_lbrt[2] + 30;
	servo_move_up_lbrt[1] = servo_move_down_lbrt[1];
	servo_move_up_lbrt[2] = servo_move_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, move_up_label, strlen(move_up_label)) + 8;
	servo_move_up_lbrt[3] = servo_move_down_lbrt[3];

	servo_done_lbrt[0] = servo_move_down_lbrt[2] + (servo_move_up_lbrt[0] - servo_move_down_lbrt[2]) / 2.0 - XPLMMeasureString(xplmFont_Proportional, done_label, strlen(done_label)) / 2.0;
	servo_done_lbrt[3] = servo_move_down_lbrt[1] - 15;
	servo_done_lbrt[2] = servo_done_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, done_label, strlen(done_label)) + 8;
	servo_done_lbrt[1] = servo_done_lbrt[3] - (1.25f * char_height);

	// Draw the boxes around our rudimentary buttons
	float white[] = { 1.0, 1.0, 1.0, 1.0 };
	glColor4fv(white);
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_move_down_lbrt[0], servo_move_down_lbrt[3]);
		glVertex2i(servo_move_down_lbrt[2], servo_move_down_lbrt[3]);
		glVertex2i(servo_move_down_lbrt[2], servo_move_down_lbrt[1]);
		glVertex2i(servo_move_down_lbrt[0], servo_move_down_lbrt[1]);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_move_up_lbrt[0], servo_move_up_lbrt[3]);
		glVertex2i(servo_move_up_lbrt[2], servo_move_up_lbrt[3]);
		glVertex2i(servo_move_up_lbrt[2], servo_move_up_lbrt[1]);
		glVertex2i(servo_move_up_lbrt[0], servo_move_up_lbrt[1]);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	{
		glVertex2i(servo_done_lbrt[0], servo_done_lbrt[3]);
		glVertex2i(servo_done_lbrt[2], servo_done_lbrt[3]);
		glVertex2i(servo_done_lbrt[2], servo_done_lbrt[1]);
		glVertex2i(servo_done_lbrt[0], servo_done_lbrt[1]);
	}
	glEnd();

	// Draw the button text (move down)
	XPLMDrawString(col_white, servo_move_down_lbrt[0] + 4, servo_move_down_lbrt[1] + 4, (char*)move_down_label, NULL, xplmFont_Proportional);

	// Draw the button text (move up)
	XPLMDrawString(col_white, servo_move_up_lbrt[0] + 4, servo_move_up_lbrt[1] + 4, (char*)move_up_label, NULL, xplmFont_Proportional);

	// Draw the button text (move up)
	XPLMDrawString(col_white, servo_done_lbrt[0] + 4, servo_done_lbrt[1] + 4, (char*)done_label, NULL, xplmFont_Proportional);
}

void drawTrimOptions(XPLMWindowID in_window_id, void* in_refcon) {




	float col_white[] = { 1.0, 1.0, 1.0 };
	XPLMSetGraphicsState(
		0 /* no fog */,
		0 /* 0 texture units */,
		0 /* no lighting */,
		0 /* no alpha testing */,
		1 /* do alpha blend */,
		1 /* do depth testing */,
		0 /* no depth writing */
	);
	int char_height;
	int char_width;
	XPLMGetFontDimensions(xplmFont_Proportional, &char_width, &char_height, NULL);
	const char* button_actuator_label;
	const char* button_input_label;
	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {
		button_input_label = elevatorTrimInputType ? encoder_label : potentiometer_label;
		switch (elevatorTrimActuatorType) {
		case 0:
			button_actuator_label = none_label;
			break;
		case 1:
			button_actuator_label = unipolar_stepper_label;
			break;
		case 2:
			button_actuator_label = bipolar_stepper_label;
			break;
		default:
			button_actuator_label = none_label;
			break;
		}
	}
	else {
		button_input_label = rudderTrimInputType ? encoder_label : potentiometer_label;
		switch (rudderTrimActuatorType) {
		case 0:
			button_actuator_label = none_label;
			break;
		case 1:
			button_actuator_label = unipolar_stepper_label;
			break;
		case 2:
			button_actuator_label = bipolar_stepper_label;
			break;
		default:
			button_actuator_label = none_label;
			break;
		}
	}



	int l, t, r, b;
	XPLMGetWindowGeometry(in_window_id, &l, &t, &r, &b);

	trim_input_lbrt[0] = l + 10 + XPLMMeasureString(xplmFont_Proportional, input_label, strlen(input_label)) + 100;
	trim_input_lbrt[3] = t - 15;
	trim_input_lbrt[2] = trim_input_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, button_input_label, strlen(button_input_label)) + 8;
	trim_input_lbrt[1] = trim_input_lbrt[3] - (1.25f * char_height);

	trim_actuator_lbrt[0] = trim_input_lbrt[0];
	trim_actuator_lbrt[3] = trim_input_lbrt[1] - 15;
	trim_actuator_lbrt[2] = trim_actuator_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, button_actuator_label, strlen(button_actuator_label)) + 8;
	trim_actuator_lbrt[1] = trim_actuator_lbrt[3] - (1.25f * char_height);

	encoder_X_up_lbrt[0] = trim_input_lbrt[0]; //- 4 * (XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) + 8)
	encoder_X_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
	encoder_X_up_lbrt[2] = encoder_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	encoder_X_up_lbrt[1] = encoder_X_up_lbrt[3] - (0.75f * char_height);

	encoder_I_up_lbrt[0] = encoder_X_up_lbrt[2];
	encoder_I_up_lbrt[3] = encoder_X_up_lbrt[3];
	encoder_I_up_lbrt[2] = encoder_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	encoder_I_up_lbrt[1] = encoder_X_up_lbrt[1];

	encoder_X_down_lbrt[0] = encoder_X_up_lbrt[0];
	encoder_X_down_lbrt[3] = encoder_X_up_lbrt[1] - (1.25f * char_height);
	encoder_X_down_lbrt[2] = encoder_X_up_lbrt[2];
	encoder_X_down_lbrt[1] = encoder_X_down_lbrt[3] - (0.75f * char_height);

	encoder_I_down_lbrt[0] = encoder_X_down_lbrt[2];
	encoder_I_down_lbrt[3] = encoder_X_down_lbrt[3];
	encoder_I_down_lbrt[2] = encoder_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	encoder_I_down_lbrt[1] = encoder_X_down_lbrt[1];

	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {

		if (elevatorTrimInputType == 0) {//if input is pot, put stepper resolution setting right under stepper type setting
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
		}
		else {
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = encoder_X_down_lbrt[1] - 15;
		}

	}
	else if (!strcmp((char*)in_refcon, "Rudder Trim options")) {

		if (rudderTrimInputType == 0) {//if input is pot, put stepper resolution setting right under stepper type setting
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = trim_actuator_lbrt[1] - 15;
		}
		else {
			stepper_M_up_lbrt[0] = trim_input_lbrt[0];
			stepper_M_up_lbrt[3] = encoder_X_down_lbrt[1] - 15;
		}

	}
	stepper_M_up_lbrt[2] = stepper_M_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_M_up_lbrt[1] = stepper_M_up_lbrt[3] - (0.75f * char_height);

	stepper_C_up_lbrt[0] = stepper_M_up_lbrt[2];
	stepper_C_up_lbrt[3] = stepper_M_up_lbrt[3];
	stepper_C_up_lbrt[2] = stepper_C_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_C_up_lbrt[1] = stepper_M_up_lbrt[1];

	stepper_X_up_lbrt[0] = stepper_C_up_lbrt[2];
	stepper_X_up_lbrt[3] = stepper_C_up_lbrt[3];
	stepper_X_up_lbrt[2] = stepper_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_X_up_lbrt[1] = stepper_C_up_lbrt[1];

	stepper_I_up_lbrt[0] = stepper_X_up_lbrt[2];
	stepper_I_up_lbrt[3] = stepper_X_up_lbrt[3];
	stepper_I_up_lbrt[2] = stepper_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_I_up_lbrt[1] = stepper_X_up_lbrt[1];

	stepper_M_down_lbrt[0] = stepper_M_up_lbrt[0];
	stepper_M_down_lbrt[3] = stepper_M_up_lbrt[1] - (1.25f * char_height);
	stepper_M_down_lbrt[2] = stepper_M_up_lbrt[2];
	stepper_M_down_lbrt[1] = stepper_M_down_lbrt[3] - (0.75f * char_height);

	stepper_C_down_lbrt[0] = stepper_M_down_lbrt[2];
	stepper_C_down_lbrt[3] = stepper_M_down_lbrt[3];
	stepper_C_down_lbrt[2] = stepper_C_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_C_down_lbrt[1] = stepper_M_down_lbrt[1];

	stepper_X_down_lbrt[0] = stepper_C_down_lbrt[2];
	stepper_X_down_lbrt[3] = stepper_C_down_lbrt[3];
	stepper_X_down_lbrt[2] = stepper_X_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_X_down_lbrt[1] = stepper_C_down_lbrt[1];

	stepper_I_down_lbrt[0] = stepper_X_down_lbrt[2];
	stepper_I_down_lbrt[3] = stepper_X_down_lbrt[3];
	stepper_I_down_lbrt[2] = stepper_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	stepper_I_down_lbrt[1] = stepper_X_down_lbrt[1];

	//current setting buttons
	current_M_up_lbrt[0] = trim_input_lbrt[0];
	current_M_up_lbrt[3] = stepper_M_down_lbrt[1] - 15;
	current_M_up_lbrt[2] = current_M_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_M_up_lbrt[1] = current_M_up_lbrt[3] - (0.75f * char_height);

	current_C_up_lbrt[0] = current_M_up_lbrt[2];
	current_C_up_lbrt[3] = current_M_up_lbrt[3];
	current_C_up_lbrt[2] = current_C_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_C_up_lbrt[1] = current_M_up_lbrt[1];

	current_X_up_lbrt[0] = current_C_up_lbrt[2];
	current_X_up_lbrt[3] = current_C_up_lbrt[3];
	current_X_up_lbrt[2] = current_X_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_X_up_lbrt[1] = current_C_up_lbrt[1];

	current_I_up_lbrt[0] = current_X_up_lbrt[2];
	current_I_up_lbrt[3] = current_X_up_lbrt[3];
	current_I_up_lbrt[2] = current_I_up_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_I_up_lbrt[1] = current_X_up_lbrt[1];

	current_M_down_lbrt[0] = current_M_up_lbrt[0];
	current_M_down_lbrt[3] = current_M_up_lbrt[1] - (1.25f * char_height);
	current_M_down_lbrt[2] = current_M_up_lbrt[2];
	current_M_down_lbrt[1] = current_M_down_lbrt[3] - (0.75f * char_height);

	current_C_down_lbrt[0] = current_M_down_lbrt[2];
	current_C_down_lbrt[3] = current_M_down_lbrt[3];
	current_C_down_lbrt[2] = current_C_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_C_down_lbrt[1] = current_M_down_lbrt[1];

	current_X_down_lbrt[0] = current_C_down_lbrt[2];
	current_X_down_lbrt[3] = current_C_down_lbrt[3];
	current_X_down_lbrt[2] = current_X_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_X_down_lbrt[1] = current_C_down_lbrt[1];

	current_I_down_lbrt[0] = current_X_down_lbrt[2];
	current_I_down_lbrt[3] = current_X_down_lbrt[3];
	current_I_down_lbrt[2] = current_I_down_lbrt[0] + XPLMMeasureString(xplmFont_Proportional, up_symbol_label, strlen(up_symbol_label)) * 0.9f;
	current_I_down_lbrt[1] = current_X_down_lbrt[1];



	// Draw the buttons
	drawButton(trim_input_lbrt);
	drawButton(trim_actuator_lbrt);
	if (!strcmp((char*)in_refcon, "Elevator Trim options")) {
		if (button_input_label == encoder_label) { //if encoder is selected as input, show encoder resolution setting
		//drawButton(encoder_X_up_lbrt);
		//drawButton(encoder_I_up_lbrt);
		//drawButton(encoder_X_down_lbrt);
		//drawButton(encoder_I_down_lbrt);
		//Draw the set encoder resolution text
			XPLMDrawString(col_white, l + 10, encoder_X_up_lbrt[1] + 4 - (1.25f * char_height), (char*)encoder_resolution_label, NULL, xplmFont_Proportional);

			//draw up symbols
			XPLMDrawString(col_white, encoder_X_up_lbrt[0], encoder_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_up_lbrt[0], encoder_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw down symbols
			XPLMDrawString(col_white, encoder_X_down_lbrt[0] + 3, encoder_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_down_lbrt[0] + 3, encoder_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw encoder resolution value
			if (elevatorTrimEncoderResolution > 9)
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 2, encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimEncoderResolution, 2, 0, 0, xplmFont_Proportional);
			else
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 1 * (char_width - 1), encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimEncoderResolution, 1, 0, 0, xplmFont_Proportional);
		}

		if (elevatorTrimActuatorType != 0) { //if stepper is selected as output - draw its settings
			//drawButton(stepper_M_up_lbrt);
			//drawButton(stepper_C_up_lbrt);
			//drawButton(stepper_X_up_lbrt);
			//drawButton(stepper_I_up_lbrt);

			//draw stepper resolution label
			XPLMDrawString(col_white, l + 10, stepper_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_resolution_label, NULL, xplmFont_Proportional);

			//draw resolution up symbols
			XPLMDrawString(col_white, stepper_M_up_lbrt[0], stepper_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_up_lbrt[0], stepper_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_up_lbrt[0], stepper_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_up_lbrt[0], stepper_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw resolution down symbols
			XPLMDrawString(col_white, stepper_M_down_lbrt[0] + 3, stepper_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_down_lbrt[0] + 3, stepper_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_down_lbrt[0] + 3, stepper_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_down_lbrt[0] + 3, stepper_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw stepper resolution value
			if (elevatorTrimActuatorType == 1) { //if unipolar - show unipolar resolution
				if (elevatorTrimUnipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimUnipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimUnipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimUnipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}
			else if (elevatorTrimActuatorType == 2) { //if bipolar - show bipolar resolution + current setting
				//draw stepper resolution label
				XPLMDrawString(col_white, l + 10, current_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_current_label, NULL, xplmFont_Proportional);

				//draw resolution up symbols
				XPLMDrawString(col_white, current_M_up_lbrt[0], current_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_up_lbrt[0], current_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_up_lbrt[0], current_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_up_lbrt[0], current_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

				//draw resolution down symbols
				XPLMDrawString(col_white, current_M_down_lbrt[0] + 3, current_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_down_lbrt[0] + 3, current_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_down_lbrt[0] + 3, current_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_down_lbrt[0] + 3, current_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

				if (elevatorTrimBipolarCurrent > 999)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3, current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarCurrent > 99)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 1 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarCurrent > 9)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 2 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 3 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarCurrent, 1, 0, 0, xplmFont_Proportional);


				if (elevatorTrimBipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (elevatorTrimBipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), elevatorTrimBipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}

		}
	}
	else if (!strcmp((char*)in_refcon, "Rudder Trim options")) {
		if (button_input_label == encoder_label) { //if encoder is selected as input, show encoder resolution setting
		//drawButton(encoder_X_up_lbrt);
		//drawButton(encoder_I_up_lbrt);
		//drawButton(encoder_X_down_lbrt);
		//drawButton(encoder_I_down_lbrt);
		//Draw the set encoder resolution text
			XPLMDrawString(col_white, l + 10, encoder_X_up_lbrt[1] + 4 - (1.25f * char_height), (char*)encoder_resolution_label, NULL, xplmFont_Proportional);

			//draw up symbols
			XPLMDrawString(col_white, encoder_X_up_lbrt[0], encoder_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_up_lbrt[0], encoder_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw down symbols
			XPLMDrawString(col_white, encoder_X_down_lbrt[0] + 3, encoder_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, encoder_I_down_lbrt[0] + 3, encoder_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw encoder resolution value
			if (elevatorTrimEncoderResolution > 9)
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 2, encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimEncoderResolution, 2, 0, 0, xplmFont_Proportional);
			else
				XPLMDrawNumber(col_white, encoder_X_up_lbrt[0] + 1 * (char_width - 1), encoder_X_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimEncoderResolution, 1, 0, 0, xplmFont_Proportional);
		}

		if (rudderTrimActuatorType != 0) { //if stepper is selected as output - draw its settings
			//drawButton(stepper_M_up_lbrt);
			//drawButton(stepper_C_up_lbrt);
			//drawButton(stepper_X_up_lbrt);
			//drawButton(stepper_I_up_lbrt);

			//draw stepper resolution label
			XPLMDrawString(col_white, l + 10, stepper_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_resolution_label, NULL, xplmFont_Proportional);

			//draw resolution up symbols
			XPLMDrawString(col_white, stepper_M_up_lbrt[0], stepper_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_up_lbrt[0], stepper_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_up_lbrt[0], stepper_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_up_lbrt[0], stepper_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

			//draw resolution down symbols
			XPLMDrawString(col_white, stepper_M_down_lbrt[0] + 3, stepper_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_C_down_lbrt[0] + 3, stepper_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_X_down_lbrt[0] + 3, stepper_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
			XPLMDrawString(col_white, stepper_I_down_lbrt[0] + 3, stepper_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

			//draw stepper resolution value
			if (rudderTrimActuatorType == 1) { //if unipolar - show unipolar resolution
				if (rudderTrimUnipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimUnipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimUnipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimUnipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}
			else if (rudderTrimActuatorType == 2) { //if bipolar - show bipolar resolution + current setting
				//draw stepper resolution label
				XPLMDrawString(col_white, l + 10, current_M_up_lbrt[1] + 4 - (1.25f * char_height), (char*)stepper_current_label, NULL, xplmFont_Proportional);

				//draw resolution up symbols
				XPLMDrawString(col_white, current_M_up_lbrt[0], current_M_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_up_lbrt[0], current_C_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_up_lbrt[0], current_X_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_up_lbrt[0], current_I_up_lbrt[1] + 2, (char*)up_symbol_label, NULL, xplmFont_Proportional);

				//draw resolution down symbols
				XPLMDrawString(col_white, current_M_down_lbrt[0] + 3, current_M_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_C_down_lbrt[0] + 3, current_C_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_X_down_lbrt[0] + 3, current_X_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);
				XPLMDrawString(col_white, current_I_down_lbrt[0] + 3, current_I_down_lbrt[1] + 2, (char*)down_symbol_label, NULL, xplmFont_Proportional);

				if (rudderTrimBipolarCurrent > 999)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3, current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarCurrent > 99)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 1 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarCurrent > 9)
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 2 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, current_M_up_lbrt[0] + 3 + 3 * (char_width - 3), current_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarCurrent, 1, 0, 0, xplmFont_Proportional);


				if (rudderTrimBipolarResolution > 999)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3, stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 4, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarResolution > 99)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 1 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 3, 0, 0, xplmFont_Proportional);
				else if (rudderTrimBipolarResolution > 9)
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 2 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 2, 0, 0, xplmFont_Proportional);
				else
					XPLMDrawNumber(col_white, stepper_M_up_lbrt[0] + 3 + 3 * (char_width - 3), stepper_M_up_lbrt[1] + 2 - (1.25f * char_height), rudderTrimBipolarResolution, 1, 0, 0, xplmFont_Proportional);
			}

		}
	}


	//draw choose input button label
	XPLMDrawString(col_white, trim_input_lbrt[0] + 4, trim_input_lbrt[1] + 4, (char*)button_input_label, NULL, xplmFont_Proportional);
	//draw choose actuator button label
	XPLMDrawString(col_white, trim_actuator_lbrt[0] + 4, trim_actuator_lbrt[1] + 4, (char*)button_actuator_label, NULL, xplmFont_Proportional);

	// Draw the choose input text 
	XPLMDrawString(col_white, l + 10, trim_input_lbrt[1] + 4, (char*)input_label, NULL, xplmFont_Proportional);
	// Draw the choose actuator text 
	XPLMDrawString(col_white, l + 10, trim_actuator_lbrt[1] + 4, (char*)actuator_label, NULL, xplmFont_Proportional);




}

void	closeAllWindows() {
	XPLMDestroyWindow(g_window);
	g_window = NULL;
}