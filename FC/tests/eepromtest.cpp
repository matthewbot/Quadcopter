#include <FC/drivers/EEPROM.h>
#include <stmos/peripherals/USART.h>

using namespace stmos;
using namespace FC;

USART out(1, 115200);
I2C i2c(1);
EEPROM eeprom(i2c, 0x50);

int main(int argc, char **argv) {	
	out.print("Writing to EEPROM!\n");
	
	const uint8_t writedata[] = { 99, 22, 44 };
	eeprom.write(EEPROM::pageAddress(0), writedata, sizeof(writedata));
	
	out.print("Begin raw read EEPROM test\n");
	
	uint8_t data[10];
	eeprom.read(EEPROM::pageAddress(0), data, sizeof(data));
	out.printf("EEPROM page 0:\n", data);
	unsigned int i;
	for (i=0; i<sizeof(data); i++) {
		out.printf("%u ", (unsigned int)data[i]);
	}
	out.print("\n");
	
	eeprom.read(EEPROM::pageAddress(1), data, sizeof(data));
	out.printf("EEPROM page 1:\n", data);
	for (i=0; i<sizeof(data); i++) {
		out.printf("%u ", (unsigned int)data[i]);
	}
	out.print("\n");
}

