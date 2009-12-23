#include <FC/drivers/ESC.h>
#include <FC/drivers/VexRC.h>
#include <stmos/peripherals/Timer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <../../stmos/include/stm32f10x.h>


using namespace FC;
using namespace stmos;

USART out(1, 115200);
Timer tim(4);
VexRC vex(tim, 4);

Timer esctim(3);
ESC south(esctim, 3);

IOPin buzzer(IOPin::PORT_C, 12, IOPin::OUTPUT_OPENDRAIN);

int main(int argc, char **argv) {
	buzzer = false;
	Task::sleep(500);
	buzzer = true;

	tim.setTickTime(720);
	tim.setOverflow(4000);
	tim.start();
	
	esctim.setTickTime(720);
	esctim.setOverflow(1000);
	esctim.start();

	while (!vex.getSynced()) { }

	south.arm();

	while (true) {
		Task::sleep(500);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();
		
			out.printf("%d\n", chans.analogs[1]);
			
			south.setThrottle(chans.analogs[1] / 50.0);
		} else {
			out.print("Not synced\n");
			south.off();
		}
	}
}

