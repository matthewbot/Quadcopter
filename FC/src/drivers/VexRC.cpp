#include "VexRC.h"
#include <stmos/util/Task.h>
#include <string.h>

using namespace FC;
using namespace stmos;

VexRC::VexRC(const stmos::Timer &tim, int channum) 
: tim(tim),
  capturechan(tim.getNum(), channum),
  synced(false),
  currentchan(0),
  prevtimerval(0),
  prevtime(Task::getCurrentTick()) {
  	memset(&channels, '\0', sizeof(channels));
  	
  	capturechan.setCallback(*this);
  	capturechan.setFilter(InputCaptureTimerChannel::FILTER_DTS32_8);
  	capturechan.enable();
}

bool VexRC::getSynced() const {
	return (Task::getCurrentTick() - prevtime) < 100;
}

VexRC::Channels VexRC::getChannels() const {
	Channels ret;
	
	capturechan.disableIRQ(); // actually pretty sure we could miss a pulse doing this :/
	ret = (const Channels &)channels;
	capturechan.enableIRQ();
	
	return ret;
}

void VexRC::call() {
	uint16_t timerval = capturechan.getCCR();
	prevtime = Task::getCurrentTick();
	
	int val = (int)timerval - (int)prevtimerval;
	if (val < 0)
		val += tim.getOverflow();
	prevtimerval = timerval;
	
	if (val % 10 >= 5)
		val = val/10 + 1;
	else
		val = val/10;
	
	if (val > 500) { // sync pulse
		if (currentchan != 0)
			synced = false;
			
		currentchan = 1;
		return;
	}
	
	if (currentchan == 0) {
		synced = false;
		return;
	}
	
	val -= 100;
	if (val < 0)
		val = 0;
	if (val > 100)
		val = 100;
		
	capturebuf[currentchan-1] = val;
	
	if (currentchan == 6) {
		synced = true;
		currentchan = 0;
		
		int i;
		for (i=0; i<4; i++) {
			channels.analogs[i] = (int8_t)capturebuf[i] - 50;
		}
		
		channels.left = captureToDigital(capturebuf[4]);
		channels.right = captureToDigital(capturebuf[5]);
	} else
		currentchan++;
}

VexRC::DigitalChannel VexRC::captureToDigital(uint8_t val) {
	if (val < 25)
		return DigitalChannel::UP;
	else if (val > 75)
		return DigitalChannel::DOWN;
	else
		return DigitalChannel::NONE;
}
