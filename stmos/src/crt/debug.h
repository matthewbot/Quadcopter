#ifndef STMOS_CRT_DEBUG_H
#define STMOS_CRT_DEBUG_H

#include <stdbool.h>
#include <stdint.h>

#define DEBUG_BAUD 115200

void debug_init();

void debug_setled(bool on);
void debug_flashled(unsigned int cycles);

void debug_print(const char *msg);
void debug_printhex(uint32_t num);

void debug_delay(unsigned int cycles);

#endif
