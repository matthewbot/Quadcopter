#include "drivers/sharedinit.h"
#include "peripherals/nvic.h"
#include "peripherals/timer.h"
#include "stm32f10x.h"

#define PRI_LOW 0 // interrupt that performs lengthy calculations
#define PRI_MED 5 // non-important interrupt
#define PRI_HIGH 10 // important interrupt
#define PRI_MAX 15 // systick only

void sharedinit_init() {
	nvic_set_priority(SysTick_IRQn, PRI_MAX); // Used to keep system time. Interrupt is very tiny, occurs only once per ms
	nvic_set_priority(EXTI9_5_IRQn, PRI_MED); // micromag drdy line. micromag waits indefinitely for us to read back its data
	nvic_set_priority(TIM2_IRQn, PRI_HIGH); // used to time external devices that don't wait for us, like the RC receiver and sonar
	nvic_set_priority(DMA1_Channel4_IRQn, PRI_MED); // handles receive buffer overruns on serial port
	nvic_set_priority(DMA1_Channel1_IRQn, PRI_LOW); // handles new ADC data. Will trigger some floating calculations
	
	timer_setup(2, 1000 / TIMER2_TPMS, TIMER2_MAXVAL, TIMER_DIRECTION_UP); // Used to time ppm and US signals
	timer_setup(4, 1000 / TIMER4_TPMS, TIMER4_MAXVAL, TIMER_DIRECTION_UP); // Used to time adc
}
