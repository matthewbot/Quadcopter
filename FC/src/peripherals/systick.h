#ifndef FC_PERIPHERALS_SYSTICK_H
#define FC_PERIPHERALS_SYSTICK_H

#include <stdint.h>

void systick_setup(int microseconds); // number of microseconds till interrupt
uint32_t systick_get();

#endif
