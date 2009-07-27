#ifndef FC_DRIVERS_MICROMAG_H
#define FC_DRIVERS_MICROMAG_H

#include <stdint.h>

enum micromag_axis {
	MICROMAG_AXIS_X=1,
	MICROMAG_AXIS_Y=2,
	MICROMAG_AXIS_Z=3
};

enum micromag_period {
	MICROMAG_PERIOD_32,
	MICROMAG_PERIOD_64,
	MICROMAG_PERIOD_128,
	MICROMAG_PERIOD_256,
	MICROMAG_PERIOD_512,
	MICROMAG_PERIOD_1024,
	MICROMAG_PERIOD_2048,
	MICROMAG_PERIOD_4096
};

void micromag_init();
int16_t micromag_sample(enum micromag_axis axis, enum micromag_period period);


#endif
