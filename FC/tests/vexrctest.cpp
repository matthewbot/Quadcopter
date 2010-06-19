#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

PPMTimer tim;
VexRC vex(tim, 4);
USART out(1, 115200);
Buzzer buzzer;

static const char *digitalToString(VexRC::DigitalChannel chan) {
	switch (chan) {
		case VexRC::UP:
			return "UP";
		case VexRC::DOWN:
			return "DOWN";
		default:
			return "NONE";
	};
}

int main(int argc, char **argv) {
	bool prevsynced=true;
		
	while (true) {
		Task::sleep(5);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();
		
			out.printf("%d %d %d %d ", chans.analogs[0], chans.analogs[1], chans.analogs[2], chans.analogs[3]);
			out.printf("%s %s\n", digitalToString(chans.left), digitalToString(chans.right));
			prevsynced = true;
		} else {
			if (prevsynced) {
				buzzer.buzz(100);
				out.print("Not synced\n");
			}
			prevsynced = false;
		}
	}
}

