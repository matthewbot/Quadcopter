#include "drivers/vexrc.h"
#include "drivers/time.h"
#include "drivers/sharedinit.h"
#include "peripherals/timer.h"
#include <stdint.h>
#include <string.h>

static void vexrc_capture_callback(int val);

static volatile uint8_t channels[6]; // stored 0-100
static volatile bool synced;

void vexrc_init() {
	timer_channel_setup_ic(2, 3, TIMER_IC_FILTER_DTS8_8, TIMER_IC_EDGE_RISING, vexrc_capture_callback);
}

struct vexrc_channels vexrc_get_channels() {
	struct vexrc_channels ret;
	ret.synced = synced;
	memcpy(ret.channels, (void *)channels, sizeof(channels));
	
	return ret;
}

bool vexrc_get_synced() {
	return synced;
}

#include <stdio.h>

static void vexrc_capture_callback(int timerval) {
	static int current_channel; // the current channel we're decoding a pulse for, 0 if we need a sync pulse next
	static int last_timerval; // the last timer value that we got a capture on (used to measure deltas)
	static unsigned long last_time; // the last time we got a capture
	
	unsigned long time = time_get();
	
	if (timerval == TIMER_IC_OVERFLOW) { // if we got an overflow event
		if (time - last_time > 30) { // if there have been no new signals for 30ms
			current_channel = 0; // start expecting a sync pulse
			synced = false; // we're desynced
		}
		return;
	}
	last_time = time;

	int val = timerval - last_timerval; // compute the real value, which is the change in timer
	if (val < 0) // handle timer wrap-arounds
		val += TIMER2_MAXVAL;	
	last_timerval = timerval;
	
	if (val > 500) { // sync pulses are longer than 5ms
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
