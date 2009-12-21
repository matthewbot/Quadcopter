#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <stmos/util/Mutex.h>

using namespace stmos;

static Mutex mutex;
static USART out(1, 115200);

class OtherTaskCallback : public Callback {
	virtual void call() {
		while (true) {
			Task::sleep(500);
			{
			Lock lock(mutex);
			out.print("Other got mutex\n");
			Task::sleep(500);
			}
			out.print("Other released mutex\n");
		}
	}
};


int main(int argc, char **argv) {
	out.print("Starting other!\n");
	
	OtherTaskCallback othercallback;
	Task other("other", KERNEL_PRIORITY_MID, othercallback);
	
	while (true) {
		Task::sleep(1000);
		{
		Lock lock(mutex);
		out.print("Main got mutex\n");
		Task::sleep(100);
		}
		out.print("Main released mutex\n");
	}
}
