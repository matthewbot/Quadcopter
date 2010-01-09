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
AnalogSensors::Calibrations calibrations = { {
	{ { 0, 2206 }, // roll
	  { 0, -2206 }, // pitch
	  { 0, -4000 } }, // yaw
	{ { 32000, -6750 }, // x-axis
	  { 32000, -6750 }, // y-axis
	  { 32000, 6750 } } // z-axis
} };	
ADC adc(1);
AnalogSensors analog(adc, chans, calibrations);

TCCompass compass(chans, calibrations.accel, adc, mag);

USART out(1, 115200);

int main(int argc, char **argv) {
	out.print("Tilt compensated compass test\n");
	
	calibrations.gyro.yaw.center = adc.sampleChannel(chans.yaw_gyro);
	
	while (true) {
		long start = Task::getCurrentTick();
		
		AnalogSensors::Readings readings = analog.getReadings();
		out.printf("%f %f\n", compass.readHeading(), readings.yaw_gyro);
		
		Task::sleep(5 - (Task::getCurrentTick() - start));
	}
}

