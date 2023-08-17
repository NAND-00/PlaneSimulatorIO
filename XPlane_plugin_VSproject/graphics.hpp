#include "XPLMDisplay.h"

//graphic parameteres
static const char* done_label = "Done";
static const char* move_up_label = "Move up";
static const char* move_down_label = "Move down";
static const char* input_label = "Input:";
static const char* potentiometer_label = "Potentiometer";
static const char* encoder_label = "Rotary Encoder";
static const char* actuator_label = "Stepper:";
static const char* unipolar_stepper_label = "Unipolar stepper motor";
static const char* bipolar_stepper_label = "Bipolar stepper motor";
static const char* none_label = "None";
static const char* encoder_resolution_label = "Encoder Resolution:";
static const char* stepper_resolution_label = "Stepper Resolution:";
static const char* stepper_current_label = "Stepper Current [mA]:";
static const char* up_symbol_label = "+";
static const char* down_symbol_label = "-";
static float servo_move_up_lbrt[4]; // left, bottom, right, top
static float servo_move_down_lbrt[4]; // left, bottom, right, top
static float servo_done_lbrt[4]; // left, bottom, right, top
static float trim_input_lbrt[4];
static float trim_actuator_lbrt[4];


static float stepper_M_up_lbrt[4];
static float stepper_M_down_lbrt[4];
static float stepper_C_up_lbrt[4];
static float stepper_C_down_lbrt[4];
static float stepper_X_up_lbrt[4];
static float stepper_X_down_lbrt[4];
static float stepper_I_up_lbrt[4];
static float stepper_I_down_lbrt[4];

static float encoder_X_up_lbrt[4];
static float encoder_X_down_lbrt[4];
static float encoder_I_up_lbrt[4];
static float encoder_I_down_lbrt[4];

static float current_M_up_lbrt[4];
static float current_M_down_lbrt[4];
static float current_C_up_lbrt[4];
static float current_C_down_lbrt[4];
static float current_X_up_lbrt[4];
static float current_X_down_lbrt[4];
static float current_I_up_lbrt[4];
static float current_I_down_lbrt[4];



// An opaque handle to the window we will create
static XPLMWindowID	g_window;

void	drawCalibrateServo(XPLMWindowID in_window_id, void* in_refcon);
void	drawTrimOptions(XPLMWindowID in_window_id, void* in_refcon);

void	closeAllWindows();