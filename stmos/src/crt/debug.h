#ifndef STMOS_CRT_DEBUG_H
#define STMOS_CRT_DEBUG_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#define DEBUG_BAUD 115200

void debug_init();

void debug_setled(bool on);
void debug_flashled(unsigned int cycles);

void debug_print(const char *msg);
void debug_write(const uint8_t *data, size_t len);
void debug_printhex(uint32_t num);

void debug_delay(unsigned int cycles);

#ifdef __cplusplus
}
#endif 

#endif
