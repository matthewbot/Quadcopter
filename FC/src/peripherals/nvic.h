#ifndef FC_DRIVERS_NVIC_H
#define FC_DRIVERS_NVIC_H

enum IRQn;

void nvic_init();

typedef void (*nvic_handler)();

#define NVIC_PRIORITY_DEAD_BITS 4
#define NVIC_PRIORITY_MAX = (0xFF >> NVIC_PRIORITY_DEAD_BITS) // 15

void nvic_register_handler(int id, nvic_handler handler);
void nvic_enable_interrupt(enum IRQn irq);
void nvic_disable_interrupt(enum IRQn irq);
void nvic_set_priority(enum IRQn irq, int priority);

#endif
