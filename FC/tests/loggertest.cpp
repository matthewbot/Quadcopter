#include <FC/control/Logger.h>
#include <FC/drivers/EEPROM.h>
#include <stmos/peripherals/USART.h>

using namespace stmos;
using namespace FC;

USART out(1, 115200);
I2C i2c(1);
EEPROM eeprom(i2c, 0x50);
Logger logger(eeprom, 0);

int main(int argc, char **argv) {	
	out.print("Begin log test!\n");
	
	const uint8_t writedata[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	unsigned int i;
	for (i=0;i<20;i++)
		logger.write(writedata, sizeof(writedata));
		
	logger.flush();
	out.print("Done!\n");
	
	uint8_t data[128];
	eeprom.read(EEPROM::pageAddress(0), data, sizeof(data));
	out.printf("EEPROM page 0:\n", data);
	for (i=0; i<sizeof(data); i++) {
		out.printf("%u ", (unsigned int)data[i]);
	}
	out.print("\n");
}

