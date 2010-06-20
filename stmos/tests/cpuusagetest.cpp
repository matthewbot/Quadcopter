#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/Timer.h>
#include <stmos/util/Task.h>

using namespace stmos;

USART out(1, 115200);

Timer timer(3);

struct DummyCallback : public Callback {
	virtual void call() {
		unsigned int i;
		for (i=0;i<400;i++) { asm("nop"); }
	}
};

DummyCallback dummycallback;

struct OtherTaskCallback : public Callback {
	virtual void call() {
		out.print("Othertask start!\n");
		while (true) {
			unsigned int i;
			for (i=0;i<4000000;i++) { asm("nop"); }
			out.print("Othertask says hi!\n");
		}
	}
};

OtherTaskCallback othertaskcallback;
Task othertask("othertask", Task::PRIORITY_MID, othertaskcallback);

int main(int argc, char **argv) {
	out.print("Hello World!\n");
	
	timer.setTickTime(72);
	timer.setOverflow(10000);
	timer.setOverflowCallback(dummycallback);
	timer.start();
	
	othertask.start();
	
	while (true) {
		unsigned int i;
		for (i=0;i<10000000;i++) { asm("nop"); }
		out.printf("main CPU usage: %f\n", Task::getCurrentTask()->getCPUUsage());
		out.printf("other CPU usage: %f\n", othertask.getCPUUsage());
		out.printf("IRQ CPU usage: %f\n", Task::getIRQCPUUsage());
	}
}
