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
			2.5E-9, 5E-7, 2.5E-8,
			5E-7, 1E-4, 5E-6,
			2.5E-8, 5E-6, 2.5E-7
		}, { // R
			.001, 0,
			0,  .005
		}
	}, { // yaw_kalman
		{ // Q
			6.25E-10, 1.25E-7, 2.5E-9,
			1.25E-7, 2.5E-5, 5E-7,
			2.5E-9, 5E-7,1E-8
		}, { // R
			.001, 0,
			0, .001
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
		out.printf("%f %f %f\n", state.roll, state.pitch, state.yaw);
	}
}

