#include <FC/drivers/EEPROM.h>
#include <stmos/peripherals/USART.h>

using namespace stmos;
using namespace FC;

USART out(1, 115200);
I2C i2c(1);
EEPROM eeprom(i2c, 0x50);

int main(int argc, char **argv) {	
	out.print("Begin EEPROM test\n");
	
	uint8_t data[30];
	eeprom.read(EEPROM::pageAddress(1), data, sizeof(data)-2);
	data[sizeof(data)-1] = '\0';
	
	out.printf("EEPROM: %s\n", data);
}

