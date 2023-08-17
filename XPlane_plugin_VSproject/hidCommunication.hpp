#include <stdint.h>

void updateFeatureBuffer(void);

//outBuffer data update functions
void updateGearState(uint8_t* buffer, int writePosition);
void updateFlapsState(uint8_t* buffer, int writePosition);
void updateElevatorTrimState(uint8_t* buffer, int writePosition);
void updateRudderTrimState(uint8_t* buffer, int writePosition);

float	updateOutBuffer(float  inFlightLoop, float inInterval, int  inRelativeToNow, void* inRefcon);