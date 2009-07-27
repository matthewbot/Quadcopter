#include "drivers/vexrc.h"
#include "peripherals/timer.h"
#include <stdint.h>
#include <string.h>

static void vexrc_capture_callback(int val);

static uint8_t channels[6]; // stored 0-100
static bool synced;

#define TIMER_MAX 3000

void vexrc_init() {
	timer_setup(2, 10, TIMER_MAX, TIMER_DIRECTION_UP); // 10 microsecond or .01 ms per tick. Overflow at 30ms
	timer_channel_setup_ic(2, 3, TIMER_IC_FILTER_DTS8_8, TIMER_IC_EDGE_RISING, vexrc_capture_callback);
}

struct vexrc_channels vexrc_get_channels() {
	struct vexrc_channels ret;
	ret.synced = synced;
	memcpy(ret.channels, channels, sizeof(channels));
	
	return ret;
}

static void vexrc_capture_callback(int timerval) {
	static int current_channel; // the current channel we're decoding a pulse for, 0 if we need a sync pulse next
	static int overflow_count; // number of events we have gotten since the previous overflow (this lets us detect signal cut-off)
	static int last_timerval; // the last timer value that we got a capture on (used to measure deltas)
	
	if (timerval == TIMER_IC_OVERFLOW) { // if we got an overflow event
		if (overflow_count == 0) { // if there have been no new signals since we got the overflow
			current_channel = 0; // start expecting a sync pulse
			synced = false; // we're desynced
		} else
			overflow_count = 0;	
		return;
	}
	overflow_count++;

	
	int val = timerval - last_timerval; // compute the real value, which is the change in timer
	if (val < 0) // handle timer wrap-arounds
		val += TIMER_MAX;	
	last_timerval = timerval;
	
	if (val > 900) { // sync pulses are longer than 9ms
		if (current_channel != 0) // if we weren't expecting a sync pulse
			synced = false; // we're still out of sync some how
		current_channel = 1; // put channel back to 1
		return;
	} 
	
	if (current_channel == 0) { // if we got a pulse when we wanted a sync pulse or a pulse past the 6th
		synced = false; // we're desynced, something is wrong
		return; // skip
	}
	
	val -= 100; // adjust value to be 0 - 100
	if (val < 0) 
		val = 0;
	if (val > 100)
		val = 100;
	
	channels[current_channel - 1] = val;
	
	if (current_channel == 6) { // if we're on 6 
		synced = true; // we've received a full cycle, so we're synced now
		current_channel = 0; // we want a sync pulse next
	} else
		current_channel++; // otherwise advance to next channel
}
