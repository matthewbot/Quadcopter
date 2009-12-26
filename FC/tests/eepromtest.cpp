#include <FC/control/Logger.h>
#include <FC/drivers/EEPROM.h>
#include <stmos/peripherals/USART.h>

using namespace stmos;
using namespace FC;

USART out(1, 115200);
I2C i2c(1);
EEPROM eeprom(i2c, 0x50);
Logger log(eeprom, 1);

int main(int argc, char **argv) {	
	out.print("Begin raw read EEPROM test\n");
	
	uint8_t data[10];
	eeprom.read(EEPROM::pageAddress(1), data, sizeof(data));
	out.printf("EEPROM page 1:\n", data);
	unsigned int i;
	for (i=0; i<sizeof(data); i++) {
		out.printf("%u ", (unsigned int)data[i]);
	}
	out.print("\n");
	
	eeprom.read(EEPROM::pageAddress(2), data, sizeof(data));
	out.printf("EEPROM page 2:\n", data);
	for (i=0; i<sizeof(data); i++) {
		out.printf("%u ", (unsigned int)data[i]);
	}
	out.print("\n");
	
	out.print("Writing to the log\n");
	for (i=0; i<100; i++) {
		log.print("Hello, world! From the log!\n");
		Task::sleep(5);
	}
	out.print("Flushing\n");
	log.flush();
	out.print("Done\n");
	
	LogReader logreader(eeprom, 1);
	
	out.print("Reading from the log\n");
	int got;
	while ((got = logreader.read(data, sizeof(data))) > 0) {
		out.send(data, got);
	}
	 
	out.printf("\n. Read ended with %d\n", got);
}

