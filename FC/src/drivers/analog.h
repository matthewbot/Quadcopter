#ifndef FC_DRIVERS_analog_H
#define FC_DRIVERS_analog_H

#include <stdint.h>

struct analog_data {
	float roll_rate;
	float pitch_rate;
	float yaw_rate;
	float roll_angle;
	float pitch_angle;
};

void analog_init();
const volatile uint16_t *analog_get_raw();
struct analog_data analog_read();

#endif
