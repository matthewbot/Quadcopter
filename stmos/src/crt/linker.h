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

typedef void (*cdtor)();
extern cdtor __ctors_start;
extern cdtor __ctors_end;
extern cdtor __dtors_start;
extern cdtor __dtors_end;

#endif
