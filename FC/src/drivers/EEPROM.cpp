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
	write(addr, NULL, 0);
	i2c.receive(i2caddr, data, len);
}

void EEPROM::write(Address addr, const uint8_t *data, size_t len) {
	int delay = WRITEDELAY - (Task::getCurrentTick() - lastwritetime);
	if (delay > 0)
		Task::sleep(delay);

	setAddress(addr);
	
	if (len) {
		i2c.send(data, len);
		lastwritetime = Task::getCurrentTick();
	}
	
	i2c.stop();
}

void EEPROM::setAddress(Address addr) {
	uint16_t addr_swapped;
	asm("rev16 %0, %1" : "=r"(addr_swapped) : "r"(addr));

	i2c.start(i2caddr);
	i2c.send((const uint8_t *)&addr_swapped, 2);
}
