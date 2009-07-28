#include "crt/vectors.h"
#include "crt/status.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

// defined elsewhere

extern int main(int argc, char **argv);

// predeclares

static void fault_nmi();
static void fault_hard();

// utilities

static void copy(uint32_t *start, uint32_t *end, const uint32_t *load);
static void fill(uint32_t *start, const uint32_t *end, uint32_t val);

// flash vector, placed at the top of the binary (flash)

struct vector_table {
	void *initial_sp;
	void (*vectors[3]);
};

__attribute__ ((section(".flash_vector")))
const struct vector_table flash_vector = {
	&__main_stack_top,
	{
		&reset,
		&fault_nmi,
		&fault_hard
	}
};

// reset function

__attribute__ ((noreturn))
void reset() {  
	status_init();

	// copy data from flash into ram
	copy(&__data_start, &__data_end, &__data_load);
	
	
	// clear out bss
	fill(&__bss_start, &__bss_end, 0);
	
	// fill heap with sentinel
	fill(&__heap_start, &__heap_end, 0xDEADBEEF);
	
	main(0, NULL);
	
	while (true) { }
}

__attribute__ ((noreturn))
static void fault_nmi() {
	status_blink_halt(BLINK_COUNT_NMI);
}

__attribute__ ((noreturn))
static void fault_hard() {
	status_blink_halt(BLINK_COUNT_HARDFAULT);
}

static void copy(uint32_t *start, uint32_t *end, const uint32_t *load) {
	while (start != end)
		*start++ = *load++;
}

static void fill(uint32_t *start, const uint32_t *end, uint32_t val) {
	while (start != end)
		*start++ = val;
}
	
