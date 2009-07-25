#ifndef FC_PERIPHERALS_TIMER_H
#define FC_PERIPHERALS_TIMER_H

#include <stdint.h>

enum timer_direction {
	TIMER_DIRECTION_UP,
	TIMER_DIRECTION_DOWN
};

enum timer_channel_mode {
	TIMER_MODE_CAPTURE,
	TIMER_MODE_OUTPUT
};

enum timer_ic_filter {
	TIMER_IC_FILTER_NONE,
	TIMER_IC_FILTER_CK_2,
	TIMER_IC_FILTER_CK_4,
	TIMER_IC_FILTER_CK_8, 
	TIMER_IC_FILTER_DTS2_6,
	TIMER_IC_FILTER_DTS2_8,
	TIMER_IC_FILTER_DTS4_6,
	TIMER_IC_FILTER_DTS4_8,
	TIMER_IC_FILTER_DTS8_6,
	TIMER_IC_FILTER_DTS8_8,
	TIMER_IC_FILTER_DTS16_5,
	TIMER_IC_FILTER_DTS16_6,
	TIMER_IC_FILTER_DTS16_8,
	TIMER_IC_FILTER_DTS32_5,
	TIMER_IC_FILTER_DTS32_6,
	TIMER_IC_FILTER_DTS32_8,
};

enum timer_ic_edge {
	TIMER_IC_EDGE_RISING,
	TIMER_IC_EDGE_FALLING
};
	

#define TIMER_IC_OVERFLOW -1

typedef void (*timer_capture_callback)(int);
typedef void (*timer_output_callback)();

void timer_init();
void timer_setup(int timer, int microsec, uint16_t maxval, enum timer_direction dir);
void timer_channel_setup_ic(int timer, int channel, enum timer_ic_filter filter, enum timer_ic_edge edge, timer_capture_callback callback);
uint16_t timer_read(int timer);

#endif
