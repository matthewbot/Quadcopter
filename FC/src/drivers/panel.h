#ifndef FC_DRIVERS_PANEL_H
#define FC_DRIVERS_PANEL_H

#include <stdbool.h>

void panel_init();

enum panel_status {
	PANEL_STATUS_BOOT, // GR
	PANEL_STATUS_TEST, // YY
	PANEL_STATUS_CALIBRATING, // GY
	PANEL_STATUS_READY, // G
	PANEL_STATUS_FAULT, // R
};

void panel_set_status(enum panel_status status);

#endif
