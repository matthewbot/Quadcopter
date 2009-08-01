#ifndef FC_DRIVERS_analog_H
#define FC_DRIVERS_analog_H

#include <stdint.h>

struct analog_readings {
	uint16_t roll_rate;
	uint16_t pitch_rate;
	uint16_t yaw_rate;
	uint16_t x_accel;
	uint16_t y_accel;
	uint16_t z_accel;
};

void analog_init();
const volatile uint16_t *analog_get_raw();
struct analog_readings analog_get_readings();

#endif
