#include <FC/control/Motors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

USART out(1, 115200);
PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

ESCTimer esctim;
Motors motors(esctim);

Buzzer buzzer;

int main(int argc, char **argv) {
	while (!vex.getSynced()) { }

	motors.arm();

	while (true) {
		Task::sleep(50);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();
	
			float throttle = chans.analogs[1] / 50.0;
			float yawcorrection = (chans.analogs[0] / 50.0) / 6;	
			float pitchcorrection = (chans.analogs[2] / 50.0) / 6;
			float rollcorrection = (-chans.analogs[3] / 50.0) / 6;
		
			out.printf("%f %f %f %f\n", rollcorrection, pitchcorrection, yawcorrection, throttle);
			
			motors.setThrottle(throttle, rollcorrection, pitchcorrection, yawcorrection);
		} else {
			out.print("Not synced\n");
			motors.off();
		}
	}
}
