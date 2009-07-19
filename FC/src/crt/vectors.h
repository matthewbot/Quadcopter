#ifndef FC_CRT_VECTORS_H
#define FC_CRT_VECTORS_H

#include <stdint.h>

__attribute__ ((noreturn))
void reset();

// defined by linker

extern uint32_t __main_stack_top;

extern uint32_t __data_start;
extern uint32_t __data_end;
extern uint32_t __data_load;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

extern uint32_t __heap_start;
extern uint32_t __heap_end;

#endif
