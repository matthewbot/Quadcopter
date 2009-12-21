#include "startup.h"
#include "linker.h"
#include "fault.h"
#include "nvic.h"
#include <stm32f10x.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

__attribute__ ((noreturn))
extern void kernel_start();

// predeclares

static void init_clock();
static void copy(uint32_t *start, uint32_t *end, const uint32_t *load);
static void fill(uint32_t *start, const uint32_t *end, uint32_t val);
static void run_ctors();

// flash vector, placed at the top of the binary (flash)

struct vector_table {
	void *initial_sp;
	void (*vectors[3])();
};

__attribute__ ((section(".flash_vector")))
const struct vector_table flash_vector = {
	&__stack_end,
	{
		&reset,
		&fault_handler,
		&fault_handler
	}
};

// reset function

__attribute__ ((noreturn))
void reset() {  
	nvic_init();
	init_clock();
	copy(&__data_start, &__data_end, &__data_load);
	fill(&__bss_start, &__bss_end, 0);
	fill(&__heap_start, &__heap_end, 0xDEADBEEF);
	
	run_ctors();
	
	kernel_start();
}

static void init_clock() {
	RCC->CR |= RCC_CR_HSEON; // enable HSE oscillator
	while (!(RCC->CR & RCC_CR_HSERDY)) { } // wait for it to come up
	
	FLASH->ACR |= FLASH_ACR_LATENCY_2; // set two flash wait states
	RCC->CFGR = RCC_CFGR_PPRE1_DIV2 |  // set the APB Low-speed prescaler to divide HCLK by 2 (72 / 2 = 36, which is the max)
	            RCC_CFGR_PLLSRC | // set PLL to multiply HSE by 9 (8Mhz * 9 = 72Mhz)
	            RCC_CFGR_PLLMULL9 |
	            RCC_CFGR_ADCPRE_DIV6; // set the ADC clock to divide PCLK2 by 6 (72Mhz / 6 = 12Mhz, max is 14Mhz)
	            
	RCC->CR |= RCC_CR_PLLON; // enable PLL
	while (!(RCC->CR & RCC_CR_PLLRDY)) { } // wait for PLL to come up
	
	RCC->CFGR |= RCC_CFGR_SW_PLL; // set PLL as system clock source
	while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL) { } // wait until the switch is complete
}

static void copy(uint32_t *start, uint32_t *end, const uint32_t *load) {
	while (start != end)
		*start++ = *load++;
}

static void fill(uint32_t *start, const uint32_t *end, uint32_t val) {
	while (start != end)
		*start++ = val;
}

static void run_ctors() {
	cdtor *ptr;
	for (ptr = &__ctors_start; ptr != &__ctors_end; ptr++) {
		(*ptr)();
	}
}

