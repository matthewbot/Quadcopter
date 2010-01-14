#include <FC/drivers/AnalogSensors.h>
#include <FC/math/Kalman.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <cmath>

using namespace FC;
using namespace stmos;

const Kalman::Config kalmanconf = {
	{ // Q
		2.5E-9, 5E-7, 2.5E-8,
		5E-7, 1E-4, 5E-6,
		2.5E-8, 5E-6, 2.5E-7
	}, { // R
		.001, 0,
		0,  .005
	}
};
Kalman kalman(kalmanconf);

const AnalogSensors::Channels chans = { {6, 5, 8, 7, 9, 11} };
AnalogSensors::Calibrations calibrations = { {
	{ { 0, -2206 }, // roll
	  { 0, 2206 }, // pitch
	  { 0, -4000 } }, // yaw
	{ { 32000, -6750 }, // x-axis
	  { 32000, -6750 }, // y-axis
	  { 32000, 6750 } } // z-axis
} };	
ADC adc(1);
AnalogSensors analog(adc, chans, calibrations);
USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Starting roll kalman test\n");
	
	while (true) {
		long start = Task::getCurrentTick();
		
		AnalogSensors::Readings readings = analog.getReadings();
		float accelangle = atan2f(readings.z_accel, readings.x_accel)+M_PI/2;
		Kalman::Measurement measurement = {{ accelangle, readings.roll_gyro }};
		kalman.step(measurement);
		
		out.printf("%f %f\n", accelangle, kalman.getState().angle);
		
		long sleepamt = 5 - (Task::getCurrentTick() - start);
		if (sleepamt > 0)
			Task::sleep(sleepamt);
	}
}

