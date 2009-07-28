#ifndef FC_DRIVERS_TIME_H
#define FC_DRIVERS_TIME_H

void time_init();
unsigned long time_get(); // time since boot in millisecond
void time_sleep(unsigned long msec); // pauses for specified number of milliseconds

#endif
