#ifndef STMOS_KERNEL_NVIC_H
#define STMOS_KERNEL_NVIC_H

#ifdef __cplusplus
extern "C" {
#endif 

#include <stdbool.h>

typedef void (*nvic_handler)();

void nvic_set_handler(int irq, nvic_handler handler);
void nvic_set_priority(int irq, int priority); // priority goes 0 to 15
void nvic_set_enabled(int irq, bool enabled);

void nvic_set_basepri(int priority); // only run interrupts that are below this priority level
void nvic_clear_basepri();

inline void nvic_enable_interrupts() { __asm volatile("cpsie i;"); }
inline void nvic_disable_interrupts() { __asm volatile("cpsid i;"); }

int nvic_get_activeirq();

inline void nvic_svc() { __asm volatile("svc 0;"); }
void nvic_pendsv();

void nvic_init();

#ifdef __cplusplus
}
#endif 

#endif
