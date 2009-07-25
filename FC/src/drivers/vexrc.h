#ifndef FC_DRIVERS_VEXRC_H
#define FC_DRIVERS_VEXRC_H

#include <stdbool.h>
#include <stdint.h>

struct vexrc_channels = {
	bool synced;
	uint8_t channels[6];
}

void vexrc_init();
struct vexrc_channels vexrc_get_channels();

#endif
