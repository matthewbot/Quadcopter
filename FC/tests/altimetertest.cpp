#include <FC/drivers/EEPROM.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/Timer.h>
#include <stmos/peripherals/I2C.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <stdint.h>

using namespace FC;
using namespace stmos;

USART out(1, 115200);
Timer mclk_tim(2);
OutputCompareTimerChannel mclk(2, 4);

I2C i2c(1);

IOPin xclr(IOPin::PORT_D, 2, IOPin::OUTPUT);

struct Coefficients {
	uint16_t C1, C2, C3, C4, C5, C6, C7;
	uint8_t A, B, C, D;

	void print(USART &out) {
		out.printf("C1 %u\n", C1);
		out.printf("C2 %u\n", C2);
		out.printf("C3 %u\n", C3);
		out.printf("C4 %u\n", C4);
		out.printf("C5 %u\n", C5);
		out.printf("C6 %u\n", C6);
		out.printf("C7 %u\n", C7);
		out.printf("A %u\n", A);
		out.printf("B %u\n", B);
		out.printf("C %u\n", C);
		out.printf("D %u\n", D);
	}
};

int main(int argc, char **argv) {	
	out.print("Altimeter test!\n");
	
	mclk_tim.setTickTime(9); // 9 = .25 microsecond per tick
	mclk_tim.setOverflow(61); // 61 = 15.25 microseconds
	mclk.setMode(OutputCompareTimerChannel::MODE_TOGGLE_MATCH); // toggle match doubles period to 30.5 microseconds, or 32.7869 khz
	mclk.setEnabled(true);
	mclk_tim.start();
	
	/*while (true) {
		xclr = false;
		Task::sleep(100);
		xclr = true;
		Task::sleep(100);
	
		const uint8_t tempcmd[] = {0xFF, 0xE8 };
		i2c.start(0x77);
		i2c.send(tempcmd, sizeof(tempcmd));
		i2c.stop();
	
		Task::sleep(100);
		
		const uint8_t readcmd = 0xFD;
		i2c.start(0x77);
		i2c.send(&readcmd, sizeof(readcmd));
		
		uint16_t buf;
		i2c.receive(0x77, (uint8_t *)&buf, sizeof(buf));
		
		out.printf("Got: %u\n", (uint16_t)(buf << 8 | buf >> 8));
	}*/
	
	
	xclr = false;
	
	uint8_t addr = 16;
	i2c.start(0x50);
	i2c.send(&addr, 1);
	i2c.stop();
	
	union {
		uint8_t u8[256];
		uint16_t u16[128];
	} buf;
	i2c.receive(0x50, buf.u8, 256);
	
	out.print("U16s:\n");
	int i;
	for (i=0; i<128;i++) {
		out.printf("%d %u\n", i, (unsigned int)buf.u16[i]);
	}
	
	out.print("U8s:\n");
	for (i=0; i<256; i++) {
		out.printf("%d %u\n", i, (unsigned int)buf.u8[i]);
	}
}

