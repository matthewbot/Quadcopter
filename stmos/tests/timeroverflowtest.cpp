#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/Timer.h>
#include <stmos/util/Callback.h>

using namespace stmos;

USART out(1, 115200);
Timer timtest(2);

static void overflowfunc();
CallbackWrapper overflowcallback(overflowfunc);

int main(int argc, char **argv) {	
	timtest.setTickTime(7200);
	timtest.setOverflow(10000);
	timtest.setOverflowCallback(overflowcallback);
	timtest.start();
	
	out.print("Started\n");
}

static void overflowfunc() {
	out.print("Overflow!\n");
}

