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

enum timer_oc_mode {
	TIMER_OC_MODE_FROZEN,
	TIMER_OC_MODE_ACTIVE_MATCH,
	TIMER_OC_MODE_INACTIVE_MATCH,
	TIMER_OC_MODE_TOGGLE_MATCH,
	TIMER_OC_MODE_FORCE_INACTIVE,
	TIMER_OC_MODE_FORCE_ACTIVE,
	TIMER_OC_MODE_PWM_1,
	TIMER_OC_MODE_PWM_2
};
	

#define TIMER_IC_OVERFLOW -1

typedef void (*timer_capture_callback)(int);
typedef void (*timer_output_callback)();

void timer_init();
void timer_setup(int timer, int microsec, uint16_t maxval, enum timer_direction dir);
void timer_channel_setup_ic(int timer, int channel, enum timer_ic_filter filter, enum timer_ic_edge edge, timer_capture_callback callback);
void timer_channel_setup_oc(int timer, int channel, enum timer_oc_mode mode, timer_output_callback callback);
void timer_channel_set_ccr(int timer, int channel, uint16_t ccr);
uint16_t timer_read(int timer);

#endif
