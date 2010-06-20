#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace stmos;

USART out(1, 115200);

struct OtherTaskCallback : public Callback {
	OtherTaskCallback() { }

	virtual void call() {
		out.print("Othertask start!\n");
		while (true) {
			asm("nop");
		}
	}
};

OtherTaskCallback othertaskcallback;
Task othertask("othertask", Task::PRIORITY_MID, othertaskcallback);

int main(int argc, char **argv) {
	out.print("Hello World!\n");
	othertask.start();
	
	while (true) {
		unsigned int i;
		for (i=0;i<10000000;i++) { asm("nop"); }
		out.printf("main CPU usage: %f\n", Task::getCurrentTask()->getCPUUsage());
		out.printf("other CPU usage: %f\n", othertask.getCPUUsage());
	}
}
