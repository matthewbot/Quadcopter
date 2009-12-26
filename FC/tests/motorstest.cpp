#include <FC/control/Motors.h>
#include <FC/drivers/VexRC.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <../../stmos/include/stm32f10x.h>


using namespace FC;
using namespace stmos;

USART out(1, 115200);
PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

ESCTimer esctim;
Motors motors(esctim);

IOPin buzzer(IOPin::PORT_C, 12, IOPin::OUTPUT_OPENDRAIN);

int main(int argc, char **argv) {
	buzzer = false;
	Task::sleep(500);
	buzzer = true;

	while (!vex.getSynced()) { }

	motors.arm();

	while (true) {
		Task::sleep(50);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();

			float yawcorrection = (chans.analogs[0] / 50.0) / 10;		
			float throttle = chans.analogs[1] / 50.0;
			float pitchcorrection = (chans.analogs[2] / 50.0) / 10;
			float rollcorrection = (chans.analogs[3] / 50.0) / 10;
		
			out.printf("%f\n", throttle);
			
			motors.setThrottle(throttle, rollcorrection, pitchcorrection, yawcorrection);
		} else {
			out.print("Not synced\n");
			motors.off();
		}
	}
}

