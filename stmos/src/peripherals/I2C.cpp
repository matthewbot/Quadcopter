#include "I2C.h"
#include <stmos/util/CriticalSection.h>
#include <stmos/crt/nvic.h>
#include <stm32f10x.h>

#define I2C_CR2_FREQ_36MHZ (I2C_CR2_FREQ_5 | I2C_CR2_FREQ_2)
#define IRQ(num) (num == 1 ? I2C1_EV_IRQn : I2C2_EV_IRQn)

using namespace stmos;

static I2C_TypeDef *const i2cs[] = {0, I2C1, I2C2 };

static I2C *objs[2];
template <int i> static void irq_callback() {
	objs[i]->irq();
}
static void (*const irq_callbacks[])() = { irq_callback<0>, irq_callback<1>};

static IOPin::PortPin pins[2][2] = {
	{ { IOPin::PORT_B, 6}, { IOPin::PORT_B, 7} },
	{ { IOPin::PORT_B, 10}, { IOPin::PORT_B, 11} }
};

I2C::I2C(int num) 
: num(num),
  scl_pin(pins[num-1][0], IOPin::OUTPUT_OPENDRAIN, IOPin::NONE, true),
  sda_pin(pins[num-1][1], IOPin::OUTPUT_OPENDRAIN, IOPin::NONE, true)
{
	objs[num-1] = this;
	RCC->APB1ENR |= (num == 1) ? RCC_APB1ENR_I2C1EN : RCC_APB1ENR_I2C2EN;
	
	I2C_TypeDef *i2c = i2cs[num];
	i2c->CR1 = I2C_CR1_SWRST;
	i2c->CR1 = 0;
	i2c->CR2 = I2C_CR2_FREQ_36MHZ | I2C_CR2_ITEVTEN;
	i2c->CCR = 180;
	i2c->TRISE = 36;
	i2c->CR1 = I2C_CR1_PE;
	
	int irq = IRQ(num);
	nvic_set_handler(irq, irq_callbacks[num-1]);
	nvic_set_enabled(irq, true);
}

void I2C::reset() {
	I2C_TypeDef *i2c = i2cs[num];
	
	i2c->CR1 = I2C_CR1_SWRST;
	i2c->CR1 = I2C_CR1_PE;
}

void I2C::start(I2C::Address addr, I2C::Mode mode) {
	if (mutex.getOwner() != Task::getCurrentTask())
		mutex.wait();
	I2C_TypeDef *i2c = i2cs[num];
	
	CriticalSection crit(IRQ(num));
	curaddress = (addr << 1) | mode;
	i2c->CR1 |= I2C_CR1_START | I2C_CR1_ACK;
	
	notifier.waitLeave(crit);
}

void I2C::send(const uint8_t *buf, size_t size) {
	I2C_TypeDef *i2c = i2cs[num];
	
	CriticalSection crit(IRQ(num));
	cursendbuf = buf;
	curbufsize = size;
	i2c->CR2 |= I2C_CR2_ITBUFEN;

	notifier.waitLeave(crit);
}

void I2C::stop() {
	I2C_TypeDef *i2c = i2cs[num];
	
	i2c->CR1 |= I2C_CR1_STOP;
	while (i2c->CR1 & I2C_CR1_STOP) { }
	mutex.release();
}

void I2C::receive(uint8_t *buf, size_t size, bool nack) {
	I2C_TypeDef *i2c = i2cs[num];
	
	CriticalSection crit(IRQ(num));
	currecvbuf = buf;
	curbufsize = size;
	curnack = nack;
	i2c->CR2 |= I2C_CR2_ITBUFEN | I2C_CR2_ITEVTEN;

	notifier.waitLeave(crit);	
}

void I2C::irq() {
	I2C_TypeDef *i2c = i2cs[num];
	
	uint32_t sr = i2c->SR1;
	if (sr & I2C_SR1_TXE && curbufsize) {
		i2c->DR = *cursendbuf++;
		if (--curbufsize == 0)
			i2c->CR2 &= ~I2C_CR2_ITBUFEN;
	} else if (sr & I2C_SR1_RXNE && curbufsize) {
		*currecvbuf++ = i2c->DR;
		
		curbufsize--;
		if (curbufsize == 1 && curnack)
			i2c->CR1 &= ~I2C_CR1_ACK;
		else if (curbufsize == 0)
			i2c->CR2 &= ~I2C_CR2_ITBUFEN;	
	} else if (sr & I2C_SR1_BTF) {
		(void)i2c->DR;
		register_irqcallback();
	} else if (sr & I2C_SR1_SB) {
		i2c->DR = curaddress;
	} else if (sr & I2C_SR1_ADDR) {
		(void)i2c->SR2;
		register_irqcallback();
	}
}

void I2C::irqcallback() {
	notifier.notify();
}

