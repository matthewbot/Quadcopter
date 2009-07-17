#ifndef FC_DRIVERS_PANEL_H
#define FC_DRIVERS_PANEL_H

#include <stdbool.h>

void panel_init();

enum panel_status {
	PANEL_STATUS_BOOT, // GYR
	PANEL_STATUS_TEST, // YR
	PANEL_STATUS_TEST_FINISHED, // Y
	PANEL_STATUS_CALIBRATING, // GY
	PANEL_STATUS_READY, // G
	PANEL_STATUS_FAULT, // R
};

void panel_set_status(enum panel_status status);
void panel_set_autopilot(bool autopilot);
void panel_set_positionlock(bool poslock);

#endif
