#include "ESC.h"
#include <stmos/util/Task.h>

using namespace stmos;
using namespace FC;

ESC::ESC(Timer &tim, int timerchan)
: chan(tim.getNum(), timerchan),
  armed(false) {
	chan.setMode(OutputCompareTimerChannel::MODE_PWM_1);
}

void ESC::arm() {
	chan.setCCR(100);
	chan.enable();
	Task::sleep(1000);
	armed = true;
}

void ESC::setThrottle(float speed) {
	if (!armed)
		return;
		
	chan.setCCR(100 + (int)(speed * 100));
}

