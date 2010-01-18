#include <FC/control/IMU.h>
#include <FC/drivers/AnalogSensors.h>
#include <stmos/peripherals/USART.h>

using namespace FC;
using namespace stmos;

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
AnalogSensors sensors(adc, chans, calibrations);

const IOPin::PortPin reset = { IOPin::PORT_C, 5 };
const IOPin::PortPin drdy = { IOPin::PORT_B, 10 };
MicroMag mag(2, reset, drdy);
TCCompass compass(chans, calibrations.accel, adc, mag);

const IMU::Config imuconf = {
	{ // roll_pitch_kalman
		{ // Q
			1E-10, 1E-7, 1E-9,
			1E-7, 1E-4, 1E-6,
			1E-9, 1E-6, 1E-8
		}, { // R
			.001, 0,
			0,  .001
		}
	}, { // yaw_kalman
		{ // Q
			6.25E-12, 1.25E-9, 2.5E-11,
			1.25E-9, 2.5E-7, 5E-9,
			2.5E-11, 5E-9,1E-10
		}, { // R
			.1, 0,
			0, .0001
		}
	}
};


IMU imu(sensors, compass, imuconf);
USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Waiting for IMU\n");
	imu.start();
	
	while (!imu.ready()) { Task::sleep(100); }
	
	while (true) {
		Task::sleep(20);
		
		IMU::State state = imu.getState();
		out.printf("%f %f\n", state.yaw, compass.readHeading());
	}
}

