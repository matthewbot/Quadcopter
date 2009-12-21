#ifndef STMOS_KERNEL_ATOMIC_H
#define STMOS_KERNEL_ATOMIC_H

#include <stdint.h>
#include <stdbool.h>

bool cmpxchg(volatile uint32_t *addr, uint32_t oldval, uint32_t newval);

#endif
