#include "I2C.h"
#include <stm32f10x.h>

#define I2C_CR2_FREQ_36MHZ (I2C_CR2_FREQ_5 | I2C_CR2_FREQ_2)

using namespace stmos;

static I2C_TypeDef *const i2cs[] = {0, I2C1, I2C2 };

static void send_start(I2C_TypeDef *i2c);
static void send_addr(I2C_TypeDef *i2c, I2C::Address addr);

static IOPin::PortPin pins[2][2] = {
	{ { IOPin::PORT_B, 6}, { IOPin::PORT_B, 7} },
	{ { IOPin::PORT_B, 10}, { IOPin::PORT_B, 11} }
};

I2C::I2C(int num) 
: num(num),
  scl_pin(pins[num-1][0], IOPin::OUTPUT_OPENDRAIN, IOPin::NONE, true),
  sda_pin(pins[num-1][1], IOPin::OUTPUT_OPENDRAIN, IOPin::NONE, true)
{
	RCC->APB1ENR |= (num == 1) ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN;
	
	I2C_TypeDef *i2c = i2cs[num];
	i2c->CR1 = I2C_CR1_SWRST;
	i2c->CR1 = 0;
	i2c->CR2 = I2C_CR2_FREQ_36MHZ;
	i2c->CCR = 180;
	i2c->TRISE = 36;
	i2c->CR1 = I2C_CR1_PE;
}

void I2C::reset() {
	I2C_TypeDef *i2c = i2cs[num];
	
	i2c->CR1 = I2C_CR1_SWRST;
	i2c->CR1 = I2C_CR1_PE;
}

void I2C::start(I2C::Address addr) {
	I2C_TypeDef *i2c = i2cs[num];
	
	send_start(i2c);
	send_addr(i2c, addr << 1);
}

void I2C::send(const I2C::Address *buf, size_t size) {
	I2C_TypeDef *i2c = i2cs[num];

	while (size--) {
		while (!(i2c->SR1 & I2C_SR1_TXE)) { }
		i2c->DR = *buf++;
	}
	
	while (!(i2c->SR1 & I2C_SR1_BTF)) { }
}

void I2C::stop() {
	I2C_TypeDef *i2c = i2cs[num];

	i2c->CR1 |= I2C_CR1_STOP;
	while (!(i2c->CR1 & I2C_CR1_STOP)) { }
}

void I2C::receive(I2C::Address addr, uint8_t *buf, size_t size) {
	I2C_TypeDef *i2c = i2cs[num];
	
	send_start(i2c);
	i2c->CR1 |= I2C_CR1_ACK;
	send_addr(i2c, (addr << 1) | 1);
	
	while (size--) {
		if (size == 0) {
			i2c->CR1 &= ~I2C_CR1_ACK;
		}
		
		while (!(i2c->SR1 & I2C_SR1_RXNE)) { }
		
		*buf++ = i2c->DR;
	}	
}

static void send_start(I2C_TypeDef *i2c) {
	i2c->CR1 |= I2C_CR1_START;
	while (!(i2c->SR1 & I2C_SR1_SB)) { }
}

static void send_addr(I2C_TypeDef *i2c, I2C::Address addr) {
	i2c->DR = addr;
	while (!(i2c->SR1 & I2C_SR1_ADDR)) { }
	(void)i2c->SR2;
}

