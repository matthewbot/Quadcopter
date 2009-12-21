#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <stmos/util/Notifier.h>

using namespace stmos;

static Notifier notifier;
static USART out(1, 115200);

struct OtherTaskCallback : public Callback {
	int id;

	OtherTaskCallback(int id) : id(id) { }

	virtual void call() {
		while (true) {
			notifier.wait();
			out.printf("Other task #%d woken up\n", id);	
		}
	}
};


int main(int argc, char **argv) {
	out.print("Starting other tasks\n");
	
	OtherTaskCallback othercallback1(1);
	Task other1("other1", KERNEL_PRIORITY_MID, othercallback1, 500);
	
	OtherTaskCallback othercallback2(2);
	Task other2("other2", KERNEL_PRIORITY_MID, othercallback2, 500);
	
	int count=0;
	while (true) {
		Task::sleep(1000);
	
		if (++count % 4 != 0) {
			out.print("Main notifying\n");
			notifier.notify();
		} else {
			count = 0;
			out.print("Main notifying all\n");
			notifier.notifyAll();
		}
	}
}
