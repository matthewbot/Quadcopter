#include <FC/drivers/ESC.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/PPMTimer.h>
#include <FC/util/ESCTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <../../stmos/include/stm32f10x.h>


using namespace FC;
using namespace stmos;

USART out(1, 115200);
PPMTimer tim;
VexRC vex(tim, 4);

ESCTimer esctim;
ESC south(esctim, 3);

Buzzer buzzer;

int main(int argc, char **argv) {
	while (!vex.getSynced()) { }

	south.arm();

	while (true) {
		Task::sleep(500);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();
		
			out.printf("%d\n", chans.analogs[1]);
			
			south.setThrottle(chans.analogs[1] / 200.0);
		} else {
			out.print("Not synced\n");
			south.off();
		}
	}
}

