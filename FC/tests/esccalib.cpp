#include <FC/drivers/ESC.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

USART out(1, 115200);
PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

ESCTimer esctim;
ESC north(esctim, 4);

int main(int argc, char **argv) {
	out.print("ESC throttle calibration\n");
	out.print("Waiting for VexRC\n");
	while (!vex.getSynced()) { Task::sleep(1); }
	
	north.enterProgramMode();
	
	out.print("Waiting for DOWN button to send 0 throttle signal\n");
	while (vex.getChannels().left != VexRC::DOWN) { Task::sleep(1); }
	
	north.setThrottle(0);
	
	out.print("Done!\n");
}

