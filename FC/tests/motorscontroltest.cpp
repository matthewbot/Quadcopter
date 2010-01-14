#include <FC/control/Motors.h>
#include <FC/control/IMU.h>
#include <FC/control/MotorsController.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

USART out(1, 115200);

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

ESCTimer esctim;
Motors motors(esctim);

const MotorsController::Config controlconfig = {
	{
		.14, 0.25, 0.03,
		.1,
		0.01
	}, {
		.08, 0.0, 0,
		.05,
		0.01
	}
};
MotorsController control(controlconfig, imu, motors, esctim);

PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

Buzzer buzzer;

int main(int argc, char **argv) {
	imu.start();

	out.print("Waiting for remote signal\n");
	while (!vex.getSynced()) { Task::sleep(100); }

	out.print("Arming\n");
	motors.arm();
	control.start();

	while (true) {
		Task::sleep(50);
		
		if (vex.getSynced()) {
			VexRC::Channels chans = vex.getChannels();
	
			float throttle = chans.analogs[1] / 50.0;
			if (throttle < 0)
				throttle = 0;
			float rollsetpoint = (-chans.analogs[3] / 50.0) * 0.3;
			
			control.setControlPoints(throttle, rollsetpoint, 0, 0);
			
			IMU::State state = imu.getState();
			out.printf("%f %f\n", state.roll, rollsetpoint, control.getRollCorrection(), control.roll_pid.int_error);
		} else {
			control.stop();
			out.print("Not synced\n");
			motors.off();
		}
	}
}
