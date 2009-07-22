#ifndef FC_DRIVERS_VEXRC_H
#define FC_DRIVERS_VEXRC_H

#include <stdbool.h>
#include <stdint.h>

struct vexrc_data {
	bool connected;
	uint32_t channels[6];
};

void vexrc_init();

struct vexrc_data vexrc_read();
const uint32_t *vexrc_read_raw();

#endif
