#include <stdint.h>

void		receive_main_monitor_bounds(int inMonitorIndex, int inLeftBx, int inTopBx, int inRightBx, int inBottomBx, void* refcon);
void		uint16To8Converter(uint16_t input, uint8_t* output, int writePosition);
int	coord_in_rect(float x, float y, float* bounds_lbrt);

void		saveConfig(const char* fileName);
void		readConfig(const char* fileName);
void		initConfig();