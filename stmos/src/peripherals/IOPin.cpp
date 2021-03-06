#include "IOPin.h"
#include <stmos/util/IRQFunction.h>
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>
#include <stdint.h>

#define CNF_ANALOG (0 << 2)
#define CNF_FLOAT (1 << 2)
#define CNF_FLOAT_PULLUPDOWN (2 << 2)

#define CNF_PUSHPULL (0 << 2)
#define CNF_OPENDRAIN (1 << 2)
#define CNF_AF_PUSHPULL (2 << 2)
#define CNF_AF_OPENDRAIN (3 << 2)

#define MODE_INPUT 0
#define MODE_OUTPUT 3

using namespace stmos;

static GPIO_TypeDef *const gpios[] = { GPIOA, GPIOB, GPIOC, GPIOD };

void IOPinConfig::setup(Port port, Pin pin, Mode mode, PullUp pullup, bool alternatefunction) {
	uint32_t config;
	
	RCC->APB2ENR |= (1 << (port+2));
	
	switch (mode) {
		case INPUT:
		default: // shut compiler up
			if (pullup != NONE)
				config = MODE_INPUT | CNF_FLOAT_PULLUPDOWN;
			else
				config = MODE_INPUT | CNF_FLOAT;
			break;
			
		case INPUT_ANALOG:
			config = MODE_INPUT | CNF_ANALOG;
			break;
			
		case OUTPUT:
			if (alternatefunction)
				config = MODE_OUTPUT | CNF_AF_PUSHPULL;
			else
				config = MODE_OUTPUT | CNF_PUSHPULL;
			break;
			
		case OUTPUT_OPENDRAIN:
			if (alternatefunction)
				config = MODE_OUTPUT | CNF_AF_OPENDRAIN;
			else
				config = MODE_OUTPUT | CNF_OPENDRAIN;
			break;
	}
	
	GPIO_TypeDef *gpio = gpios[port];
	if (pin < 8) {
		uint32_t crl = gpio->CRL;
		int shiftamt = pin*4;
		
		crl &= ~(0x0f << shiftamt);
		crl |= config << shiftamt;
		
		gpio->CRL = crl;
	} else {
		uint32_t crh = gpio->CRH;
		int shiftamt = (pin-8)*4;
		
		crh &= ~(0x0f << shiftamt);
		crh |= config << shiftamt;
		
		gpio->CRH = crh;
	}
	
	if (mode == INPUT && pullup != NONE) {
		uint32_t mask = 1 << pin;
		if (pullup == PULLUP)
			gpio->ODR &= ~mask;
		else // PULLDOWN
			gpio->ODR |= mask;
	}
}

IOPin::IOPin(Port port, Pin pin, Mode mode, PullUp pullup, bool alternatefunction)
: IOPinConfig(port, pin, mode, pullup, alternatefunction), port(port), pin(pin) { }

IOPin::IOPin(const PortPin &portpin, Mode mode, PullUp pullup, bool alternatefunction)
: IOPinConfig(portpin, mode, pullup, alternatefunction), port(portpin.port), pin(portpin.pin) { }

bool IOPin::read() const {
	GPIO_TypeDef *gpio = gpios[port];
	return (gpio->IDR >> pin) & 1;
}

void IOPin::set(bool val) {
	GPIO_TypeDef *gpio = gpios[port];
	gpio->BSRR = 1 << (val ? pin : pin+16);
}

void IOPin::pulse() {
	int i;
	set(true);
	for (i=0;i<10;i++) { asm volatile("nop"); }
	set(false);
}

static Callback *exti_callbacks[16];
static void exti_irq_handler();

int IOPin::getIRQ() {
	if (pin <= 4)
		return EXTI0_IRQn + pin;
	else if (pin <= 9)
		return EXTI9_5_IRQn;
	else
		return EXTI15_10_IRQn;
}

void IOPin::setIRQHandler(Callback &callback, int pri) {
	RCC->APB2ENR |= RCC_APB2ENR_AFIOEN;

	exti_callbacks[pin] = &callback;
	
	__IO uint32_t *const exticr = &AFIO->EXTICR[pin >> 2]; // divide exti by 4 to get which register it belongs in
	const int exticr_shiftamt = 4 * (pin & 0x03); // the lower two bits determine its position in its register

	*exticr &= ~(0x0F << exticr_shiftamt); // clear exticr settings
	*exticr |= port << exticr_shiftamt;
	
	int irq = getIRQ();
	nvic_set_handler(irq, exti_irq_handler);
	nvic_set_priority(irq, pri);
	nvic_set_enabled(irq, true);
}

void IOPin::setIRQEnabled(Edge edge) {
	const uint32_t exti_bit = (1 << pin);

	if (edge & EDGE_RISING)
		EXTI->RTSR |= exti_bit;
	else
		EXTI->RTSR &= ~exti_bit;
		
	if (edge & EDGE_FALLING)
		EXTI->FTSR |= exti_bit;
	else
		EXTI->FTSR &= ~exti_bit;
		
	if (edge != 0)
		EXTI->IMR |= exti_bit;
	else
		EXTI->IMR &= ~exti_bit;
}

static void run_exti_callback(int irq) {
	Callback *callback = exti_callbacks[irq];
	if (callback)
		callback->call();
	
	EXTI->PR = (1 << irq);
}

static void exti_irq_handler() {
	IRQFunction irqfunc;
	int irq;
	const uint32_t PR = EXTI->PR;
	
	for (irq=0; irq<=15; irq++) {
		if (PR & (1 << irq)) {
			run_exti_callback(irq);
			break;
		}
	}
}

IOPinWait::IOPinWait(Port port, Pin pin, PullUp pullup)
: IOPin(port, pin, INPUT, pullup) {
	setIRQHandler(*this);
}

IOPinWait::IOPinWait(const PortPin &portpin, PullUp pullup)
: IOPin(portpin, INPUT, pullup) {
	setIRQHandler(*this);
}

void IOPinWait::wait(Edge edge) {
	CriticalSection crit(getIRQ());
	setIRQEnabled(edge);

	bool val = read();
	if (val) {
		if (edge == EDGE_RISING)
			return;
	} else {
		if (edge == EDGE_FALLING)
			return;
	}

	notifier.waitLeave(crit);
	disableIRQ();
}

void IOPinWait::call() {
	register_irqcallback();
}

void IOPinWait::irqcallback() {
	notifier.notifyAll();
}
