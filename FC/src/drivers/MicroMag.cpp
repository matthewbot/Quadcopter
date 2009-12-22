#include "MicroMag.h"
#include <stmos/util/Task.h>
#include <stdint.h>

using namespace FC;
using namespace stmos;

extern "C" { 
#include <stmos/crt/debug.h> 
}

MicroMag::MicroMag(int spinum, IOPin::Port reset_port, int reset_pinnum, IOPin::Port drdy_port, int drdy_pinnum)
: spi(spinum, SPI::BAUDDIV_128),
  reset_pin(reset_port, reset_pinnum, IOPin::OUTPUT),
  drdy_pin(drdy_port, drdy_pinnum, IOPin::INPUT),
  scan_tottime(0),
  scan_axis(_AXIS_NONE) {
  
  drdy_pin.setupEXTI(*this);
}

MicroMag::Result MicroMag::sample(Axis axis, Period period) {
	scan_axis = _AXIS_NONE; // shut off any in-progress scan
	
	query(axis, period);
	while (!drdy_pin) { Task::sleep(2); }
	return getResult();
}

void MicroMag::startScan(Period period) {
	scan_period = period;
	scan_axis = AXIS_X;
	scan_prevtime = Task::getCurrentTick();
	scan_callback = NULL;
	
	query(scan_axis, period);
}

void MicroMag::startScan(Period period, Callback &callback) {
	startScan(period);
	scan_callback = &callback;
}

void MicroMag::stopScan() {
	scan_axis = _AXIS_NONE;
}

void MicroMag::query(Axis axis, Period period) {
	reset_pin.pulse();
	
	uint8_t command = axis | (period << 4);
	spi.send(&command, 1);
}

MicroMag::Result MicroMag::getResult() {
	uint8_t bytes[2];
	spi.receive(bytes, 2);
	
	return (Result)((bytes[0] << 8) | bytes[1]);
}

void MicroMag::call() {
	if (scan_axis == _AXIS_NONE)
		return;
		
	Result result = getResult();
	
	switch (scan_axis) {
		default:
		case AXIS_X:
			scan_results.x = result;
			break;
			
		case AXIS_Y:
			scan_results.y = result;
			break;
			
		case AXIS_Z:
			scan_results.z = result;
			scan_axis = _AXIS_NONE;
			
			unsigned long curtime = Task::getCurrentTick();
			scan_tottime = curtime - scan_prevtime;
			scan_prevtime = curtime;
			
			if (scan_callback)
				scan_callback->call();
	}
	
	scan_axis = (Axis)((int)scan_axis + 1);
	
	query(scan_axis, scan_period);
}

