#include "peripherals/exti.h"
#include "peripherals/nvic.h"
#include "stm32f10x.h"

static exti_handler handlers[19];

static void exti_irq_handler_0();
static void exti_irq_handler_1();
static void exti_irq_handler_2();
static void exti_irq_handler_3();
static void exti_irq_handler_4();
static void exti_irq_handler_5_9();
static void exti_irq_handler_10_15();
static void exti_irq_handler_search(int begin, int end);
static void exti_irq_handler(int irq);

void exti_init() {
	nvic_register_handler(EXTI0_IRQn, exti_irq_handler_0, true);
	nvic_register_handler(EXTI1_IRQn, exti_irq_handler_1, true);
	nvic_register_handler(EXTI2_IRQn, exti_irq_handler_2, true);
	nvic_register_handler(EXTI3_IRQn, exti_irq_handler_3, true);
	nvic_register_handler(EXTI4_IRQn, exti_irq_handler_4, true);	
	nvic_register_handler(EXTI9_5_IRQn, exti_irq_handler_5_9, true);
	nvic_register_handler(EXTI15_10_IRQn, exti_irq_handler_10_15, true);
}

void exti_setup(int exti, enum gpio_port port, enum exti_edge edge, exti_handler handler) {
	handlers[exti] = handler;

	{ // dummy block scoping local variables
		__IO uint32_t *const exticr_addr = &AFIO->EXTICR[exti >> 2]; // divide exti by 4 to get which register it belongs in
		uint32_t exticr = *exticr_addr;
	
		const int shiftamt = 4 * (exti & 0x03); // the lower two bits determine its position in its register
	
		exticr &= ~(0x0F << shiftamt);
		exticr |= port << shiftamt;
	
		*exticr_addr = exticr;
	}
	
	{
		const uint32_t exti_bit = (1 << exti);
		
		if (edge & EXTI_EDGE_RISING) 
			EXTI->RTSR |= exti_bit;
		else
			EXTI->RTSR &= ~exti_bit;
		
		if (edge & EXTI_EDGE_FALLING)
			EXTI->FTSR |= exti_bit;
		else
			EXTI->FTSR &= ~exti_bit;
			
		EXTI->IMR |= exti_bit;
	}
}

static void exti_irq_handler(int irq) {
	exti_handler handler = handlers[irq];
	if (handler)
		handler();
		
	EXTI->PR = (1 << irq); // clear interrupt
}

static void exti_irq_handler_0() {
	exti_irq_handler(0);
}

static void exti_irq_handler_1() {
	exti_irq_handler(1);
}

static void exti_irq_handler_2() {
	exti_irq_handler(2);
}

static void exti_irq_handler_3() {
	exti_irq_handler(3);
}

static void exti_irq_handler_4() {
	exti_irq_handler(4);
}

static void exti_irq_handler_search(int begin, int end) {
	int i;
	for (i=begin; i<=end; i++) {
		if (EXTI->PR & (1 << i)) {
			exti_irq_handler(i);
			break;
		}
	}
}

static void exti_irq_handler_5_9() {
	exti_irq_handler_search(5, 9);
}

static void exti_irq_handler_10_15() {
	exti_irq_handler_search(10, 15);
}
