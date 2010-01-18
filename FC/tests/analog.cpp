#include <FC/drivers/AnalogSensors.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <cmath>

using namespace FC;
using namespace stmos;

ADC adc(1);
AnalogSensors::Calibrations calibrations = configs::analog;
AnalogSensors analog(adc, configs::chans, calibrations);
USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Calibrating gyros\n");
	
	calibrations.gyro.roll.center = adc.sampleChannel(configs::chans.roll_gyro);
	calibrations.gyro.pitch.center = adc.sampleChannel(configs::chans.pitch_gyro);
	calibrations.gyro.yaw.center = adc.sampleChannel(configs::chans.yaw_gyro);
	
	while (true) {
		Task::sleep(500);
		AnalogSensors::Readings readings = analog.getReadings();
		
		out.printf("Roll %f\nPitch %f\nYaw %f\n",
		           readings.roll_gyro, readings.pitch_gyro, readings.yaw_gyro);
		out.printf("XAcc %f\nYAcc %f\nZAcc %f\n", 
		           readings.x_accel, readings.y_accel, readings.z_accel);
	}
}
