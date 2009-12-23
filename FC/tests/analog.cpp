#include <FC/drivers/AnalogSensors.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <cmath>

using namespace FC;
using namespace stmos;

const AnalogSensors::Channels chans = { {6, 5, 8, 7, 9, 11} };
AnalogSensors::Calibrations calibrations = { {
	{ { 0, 1103.288 }, // roll
	  { 0, -1103.288 }, // pitch
	  { 0, -2000 } }, // yaw
	{ { 32000, -6700 }, // x-axis
	  { 32000, -6700 }, // y-axis
	  { 32000, 6700 } } // z-axis
} };
	
AnalogSensors analog(1, chans, calibrations);
USART out(1, 115200);
IOPin roll(IOPin::PORT_A, 6, IOPin::INPUT_ANALOG);
IOPin pitch(IOPin::PORT_A, 5, IOPin::INPUT_ANALOG);
IOPin yaw(IOPin::PORT_B, 0, IOPin::INPUT_ANALOG);
IOPin x_acc(IOPin::PORT_A, 7, IOPin::INPUT_ANALOG);
IOPin y_acc(IOPin::PORT_B, 1, IOPin::INPUT_ANALOG);
IOPin z_acc(IOPin::PORT_C, 2, IOPin::INPUT_ANALOG);

int main(int argc, char **argv) {	
	out.print("Calibrating gyros\n");
	
	ADC &adc = analog.getADC();
	calibrations.gyro.roll.center = adc.sampleChannel(chans.roll_gyro);
	calibrations.gyro.pitch.center = adc.sampleChannel(chans.pitch_gyro);
	calibrations.gyro.yaw.center = adc.sampleChannel(chans.yaw_gyro);
	
	while (true) {
		Task::sleep(500);
		AnalogSensors::Readings readings = analog.getReadings();
		
		out.printf("Roll %f\nPitch %f\nYaw %f\n",
		           readings.roll_gyro, readings.pitch_gyro, readings.yaw_gyro);
		out.printf("XAcc %f\nYAcc %f\nZAcc %f\n", 
		           readings.x_accel, readings.y_accel, readings.z_accel);
	}
}
