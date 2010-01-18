#include <FC/control/TCCompass.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

MicroMag mag(2, (IOPin::PortPin) { IOPin::PORT_C, 5 }, (IOPin::PortPin)  { IOPin::PORT_B, 10 });

ADC adc(1);
AnalogSensors::Calibrations calibrations = configs::analog;
AnalogSensors analog(adc, configs::chans, calibrations);

TCCompass compass(configs::chans, configs::analog.accel, adc, mag);

USART out(1, 115200);

int main(int argc, char **argv) {
	out.print("Tilt compensated compass test\n");
	
	calibrations.gyro.yaw.center = adc.sampleChannel(configs::chans.yaw_gyro);
	
	while (true) {
		long start = Task::getCurrentTick();
		
		AnalogSensors::Readings readings = analog.getReadings();
		out.printf("%f %f\n", compass.readHeading(), readings.yaw_gyro);
		
		Task::sleep(5 - (Task::getCurrentTick() - start));
	}
}

