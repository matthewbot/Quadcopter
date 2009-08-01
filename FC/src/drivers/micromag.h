#ifndef FC_DRIVERS_MICROMAG_H
#define FC_DRIVERS_MICROMAG_H

#include <stdint.h>
#include <stdbool.h>

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

struct micromag_scan_results {
	int16_t x, y, z;
};

typedef void (*micromag_scan_handler)(struct micromag_scan_results);

void micromag_init();
int16_t micromag_sample(enum micromag_axis axis, enum micromag_period period);
void micromag_scan(enum micromag_period period, micromag_scan_handler handler);
struct micromag_scan_results micromag_get_scan();
unsigned long micromag_get_scan_time(); // in ms
bool micromag_scan_ok(); // returns false if our automatic scan locks up


#endif
