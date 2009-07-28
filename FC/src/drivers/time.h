#ifndef FC_DRIVERS_TIME_H
#define FC_DRIVERS_TIME_H

struct time_highres {
	unsigned long msecs;
	unsigned long cycles;
};

void time_init();
unsigned long time_get(); // time since boot in millisecond
void time_sleep(unsigned long msec); // pauses for specified number of milliseconds

struct time_highres time_get_highres();
struct time_highres time_highres_subtract(struct time_highres timea, struct time_highres timeb);
void time_highres_print(struct time_highres time);
#endif
