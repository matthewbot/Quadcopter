#include "math/matrix.h"
#include "drivers/micromag.h"
#include "drivers/time.h"
#include "drivers/panel.h"
#include "system.h"
#include <stdio.h>

int main(int argc, char **argv) {
	system_init();
	printf("Eh?\n");

	micromag_scan(MICROMAG_PERIOD_1024, NULL);
	
	for (;;) {
		time_sleep(500);
		printf("Micromag ok: %d\n", (int)micromag_scan_ok());
	}
}

