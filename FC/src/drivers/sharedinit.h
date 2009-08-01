#ifndef FC_DRIVERS_SHAREDINIT_H
#define FC_DRIVERS_SHAREDINIT_H

void sharedinit_init();

// shared peripheral configurations

#define TIMER2_TPMS 100 // ticks per millisecond
#define TIMER2_MAXVAL 4000 // 40 ms

#define TIMER4_TPMS 1000
#define TIMER4_MAXVAL 5000 // 5 ms

#endif
