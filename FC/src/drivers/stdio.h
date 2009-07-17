#ifndef FC_DRIVERS_STDIO_H
#define FC_DRIVERS_STDIO_H

// Called from newlib stubs! Don't make any syscalls!

#include <stdint.h>
#include <stddef.h>

void stdio_init();
size_t stdio_read(uint8_t *buf, size_t len);
size_t stdio_write(const uint8_t *buf, size_t len);

#endif
