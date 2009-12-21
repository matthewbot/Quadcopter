#ifndef STMOS_CRT_FAULT_H
#define STMOS_CRT_FAULT_H

#define BLINK_COUNT_NMI 2
#define BLINK_COUNT_HARDFAULT 3

void fault_handler();
void unhandled_fault_handler();

#endif
