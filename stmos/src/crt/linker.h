#ifndef STMOS_CRT_LINKER_H
#define STMOS_CRT_LINKER_H

#include <stdint.h>

// defined by linker

extern uint32_t __stack_start;
extern uint32_t __stack_end;

extern uint32_t __data_start;
extern uint32_t __data_end;
extern uint32_t __data_load;

extern uint32_t __bss_start;
extern uint32_t __bss_end;

extern uint32_t __heap_start;
extern uint32_t __heap_end;

typedef void (*ctor)();
extern ctor __ctors_start;
extern ctor __ctors_end;

void __run_ctors();

#endif
