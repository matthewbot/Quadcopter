#include "IOPin.h"
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

IOPinConfig::IOPinConfig(Port port, Pin pin, Mode mode, PullUp pullup, bool alternatefunction) {
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

bool IOPin::read() const {
	GPIO_TypeDef *gpio = gpios[port];
	return (gpio->IDR >> pin) & 1;
}

void IOPin::set(bool val) const {
	GPIO_TypeDef *gpio = gpios[port];
	gpio->BSRR = 1 << (val ? pin : pin+16);
}

