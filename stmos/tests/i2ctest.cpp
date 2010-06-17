#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/I2C.h>
#include <stmos/util/Task.h>

using namespace stmos;

USART out(1, 115200);
I2C i2c(1);

int main(int argc, char **argv) {	
	out.print("I2C test!\n");
	
	out.print("Send data\n");
	i2c.start(0x50);
	const uint8_t sendbuf[] = {0, 0, 108, 88, 77};
	i2c.send(sendbuf, sizeof(sendbuf));
	i2c.stop();
	
	Task::sleep(10);
	
	out.print("Set address\n");
	uint8_t recvbuf[5];
	i2c.start(0x50);
	i2c.send(sendbuf, 2);
	
	out.print("Read\n");
	i2c.receive(0x50, recvbuf, sizeof(recvbuf));
	
	unsigned int i;
	for (i=0;i<sizeof(recvbuf);i++)
		out.printf("%d ", (int)recvbuf[i]);
	out.print("\n");
	
	out.print("Done!\n");
}

