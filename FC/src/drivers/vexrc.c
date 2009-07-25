#include "drivers/vexrc.h"
#include "peripherals/timer.h"
#include <stdint.h>

static void vexrc_capture_callback(uint16_t val);

static uint8_t channels[6]; // stored 0-100
static int current_channel; // the current channel we're decoding a pulse for, 0 if we have no sync

void vexrc_init() {
	timer_setup(2, 10, 3000, TIMER_DIRECTION_UP); // 10 microsecond or .01 ms per tick. Overflow at 30ms
	timer_channel_setup_ic(2, 3, TIMER_IC_FILTER_CK_4, TIMER_IC_EDGE_RISING, vexrc_capture_callback);
}

struct vexrc_channels vexrc_get_channels() {
	struct vexrc_channels ret;
	ret.synced = (current_channel > 0);
	memcpy(ret.channels, channels, sizeof(channels));
	
	return ret;
}

static void vexrc_capture_callback(uint16_t val) {
	static int current_channel;

	if (val == TIMER_IC_OVERFLOW) { // a really long pulse 
		current_channel = 0; // lost sync
		return;
	}

	if (val > 600) { // sync pulses are longer than 6ms
		current_channel = 1;
		return;
	} 
	
	if (current_channel == 0 || current_channel > 6) // if we're not synced, or are for whatever reason getting an "extra" channel
		return; // skip
	
	if (val < 50) // cap range, full positive is a 1.5ms pulse, full negative a .5ms, but our timing may be off by a tick or two
		val = 50;
	if (val > 150)
		val = 150;
	val -= 50; // shift range to 0-100
	
	vexrc_channels[current_channel++ - 1] = val; // update channel
}
