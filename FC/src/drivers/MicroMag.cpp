#include "MicroMag.h"
#include <stmos/util/Task.h>
#include <stdint.h>

using namespace FC;
using namespace stmos;

MicroMag::MicroMag(int spinum, IOPin::PortPin reset, IOPin::PortPin drdy)
: spi(spinum, SPI::BAUDDIV_128),
  reset_pin(reset, IOPin::OUTPUT),
  drdy_pin(drdy) {

}

#include <stmos/crt/debug.h>

MicroMag::Result MicroMag::sample(Axis axis, Period period) {
	reset_pin.pulse();
	uint8_t command = (axis+1) | (period << 4);
	spi.send(&command, 1);
	
	drdy_pin.wait();
	uint8_t bytes[2];
	spi.receive(bytes, 2);
	
	return (Result)((bytes[0] << 8) | bytes[1]);
}

MicroMag::Scan MicroMag::scan(Period period) {
	Scan result;
	result.x = sample(AXIS_X, period);
	result.y = sample(AXIS_Y, period);
	result.z = sample(AXIS_Z, period);
	
	return result;
}

