#include "atomic.h"

bool cmpxchg(volatile uint32_t *addr, uint32_t oldval, uint32_t newval) {
	bool ret;

	asm (
		"begin: ldrex r3, [%[addr]];"
		"cmp r3, %[oldval];"
		"bne fail;"
		"strex r3, %[newval], [%[addr]];"
		"cmp r3, 0;"
		"bne begin;"
		"mov %[ret], 1;"
		"b end;"
		
		"fail:"
		"clrex;"
		"mov %[ret], 0;"
		
		"end:"
		: [ret] "=r" (ret)
		: [addr] "r" (addr), [oldval] "r" (oldval), [newval] "r" (newval)
		: "r3"
	);
	
	return ret;
}
