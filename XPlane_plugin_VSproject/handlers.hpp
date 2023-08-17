#include "XPLMDisplay.h"

//event handlers for windows and menus

void				mainMenuFuncHandler(void* mRef, void* iRef);
void				calibrateServoPosition(void* mRef, void* iRef);
void				trimOptions(void* mRef, void* iRef);

int					handle_mouse_servo_calibration(XPLMWindowID in_window_id, int x, int y, XPLMMouseStatus is_down, void* in_refcon);
int					handle_mouse_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon);
int					handle_mouse_rudder_trim_options(XPLMWindowID in_window_id, int x, int y, int is_down, void* in_refcon);