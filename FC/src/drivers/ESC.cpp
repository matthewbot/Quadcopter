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
	chan.setCCR(1000);
	chan.enable();
	Task::sleep(1000);
	armed = true;
}

void ESC::setThrottle(float speed) {
	if (!armed)
		return;
		
	chan.setCCR(1000 + (int)(speed * 1000));
}

float ESC::getThrottle() {
	return (chan.getCCR() - 1000) / 1000.0;
}

void ESC::enterProgramMode() {
	chan.setCCR(2000);
	chan.enable();
	armed = true;
}

