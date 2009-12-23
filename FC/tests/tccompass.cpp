#include <FC/control/TCCompass.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

const IOPin::PortPin reset = { IOPin::PORT_C, 5 };
const IOPin::PortPin drdy = { IOPin::PORT_B, 10 };
MicroMag mag(2, reset, drdy);

const AnalogSensors::Channels chans = { {6, 5, 8, 7, 9, 11} };
AnalogSensors::AccelerometerCalibration calibrations = {
      { 32000, -6750 }, // x-axis
	  { 32000, -6750 }, // y-axis
	  { 32000, 6750 } // z-axis
};	
ADC adc(1);

TCCompass compass(chans, calibrations, adc, mag);

USART out(1, 115200);

int main(int argc, char **argv) {
	out.print("Tilt compensated compass test\n");
	
	while (true) {
		Task::sleep(500);
	
		out.printf("Heading: %f\n", compass.readHeading());
	}
}

