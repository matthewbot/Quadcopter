#include "EEPROM.h"
#include <stmos/util/Task.h>

#define WRITEDELAY 6

using namespace FC;
using namespace stmos;

EEPROM::EEPROM(I2C &i2c, I2C::Address i2caddr) 
: i2c(i2c),
  i2caddr(i2caddr),
  lastwritetime(0) { }
  
void EEPROM::read(Address addr, uint8_t *data, size_t len) {
	waitReady();
	setAddress(addr);
	
	i2c.start(i2caddr, I2C::RECEIVE);
	i2c.receive(data, len, true);
	i2c.stop();
}

void EEPROM::write(Address addr, const uint8_t *data, size_t len) {
	waitReady();
	setAddress(addr);
	
	i2c.send(data, len);
	lastwritetime = Task::getCurrentTick();
	i2c.stop();
}

bool EEPROM::ready() const {
	return WRITEDELAY - (Task::getCurrentTick() - lastwritetime) <= 0;
}

void EEPROM::waitReady() const {
	while (true) {
		int delay = WRITEDELAY - (Task::getCurrentTick() - lastwritetime);
		if (delay > 0)
			Task::sleep(delay);	
		else
			break;
	}
}

void EEPROM::setAddress(Address addr) {
	uint16_t addr_swapped;
	asm("rev16 %0, %1" : "=r"(addr_swapped) : "r"(addr));

	i2c.start(i2caddr, I2C::SEND);
	i2c.send((const uint8_t *)&addr_swapped, 2);
}

