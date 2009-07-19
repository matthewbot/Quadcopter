#include "crt/vectors.h"
#include "stm32f10x.h"
#include <stdint.h>
#include <stdbool.h>

// defined elsewhere

extern void main();

// predeclares

static void fault();

// utilities

static void copy(const uint32_t *start, const uint32_t *end, uint32_t *load);
static void fill(uint32_t *start, const uint32_t *end, uint32_t val);
static void status_init();
static void status_on();
static void status_off();

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
		&fault,
		&fault,
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
	
	main();
	
	while (true) { }
}

__attribute__ ((noreturn))
static void fault() {
	status_on();
	while (true) { }
}

static void copy(const uint32_t *start, const uint32_t *end, uint32_t *load) {
	while (start != end)
		*load++ = *start++;
}

static void fill(uint32_t *start, const uint32_t *end, uint32_t val) {
	while (start != end)
		*start++ = val;
}

static void status_init() {
	GPIOC->CRH = 0x44464444; // make pin 12 open drain output 
	status_off();
}

static void status_on() {
	GPIOC->BSRR = (1 << 12); // make pin 12 high
}
	
static void status_off() {
	GPIOC->BSRR = (1 << (12+16)); // make pin 12 low
}
	
