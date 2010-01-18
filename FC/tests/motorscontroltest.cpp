#include <FC/control/Motors.h>
#include <FC/control/IMU.h>
#include <FC/control/MotorsController.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/drivers/BatteryMonitor.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <cstdio>
#include <cmath>

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

ESCTimer esctim;
Motors motors(esctim);

MotorsController::Config controlconfig = {
	{
	    0.03, 0.01, 0.025,
		1,
		ESCTimer::UPDATETIME / 1000.0
	}, {
		.03, 0, .02,
		.1,
		ESCTimer::UPDATETIME / 1000.0
	}
};
MotorsController control(controlconfig, imu, motors, esctim);

PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

Buzzer buzzer;
BatteryMonitor batmon(adc, 13);

static void docalibrate(const char *type, PID::Config *pid);

int main(int argc, char **argv) {
	imu.start();

	out.printf("Battery: %f cell\n", batmon.getCellVoltage());
	out.print("Waiting for remote signal\n");
	while (!vex.getSynced()) { Task::sleep(100); }

	out.print("Arming\n");
	motors.arm();

	float heading = imu.getYaw();

	while (true) {
		control.start();
		
		while (true) {
			if (!vex.getSynced()) {
				control.stop();
				motors.off();
				while (!vex.getSynced()) { Task::sleep(100); }
				control.start();
			}
		
			VexRC::Channels chans = vex.getChannels();
	
			if (chans.left != VexRC::NONE || chans.right != VexRC::NONE)
				break;
	
			float throttle = chans.analogs[1] / 50.0;
			if (throttle < 0)
				throttle = 0;
			float rollsetpoint = (-chans.analogs[3] / 50.0) * 0.3;
			float pitchsetpoint = (-chans.analogs[2] / 50.0) * 0.3;
			heading += (chans.analogs[0] / 50.0) * (M_PI / 25);
			
			control.setControlPoints(throttle, rollsetpoint, pitchsetpoint, heading);
			
			IMU::State state = imu.getState();
			//out.printf("%f %f %f %f\n", state.roll, state.pitch, state.yaw, batmon.getCellVoltage());
			//out.printf("%f %f %f\n", state.yaw, compass.readHeading(), batmon.getCellVoltage());
			//out.printf("%f\n", motors.getNorthThrottle());
			out.printf("%f %f %f\n", control.roll_pid.int_error, control.pitch_pid.int_error, control.yaw_pid.int_error);
			Task::sleep(50);
		}
		
		control.stop();
		motors.off();
		out.print("Push enter\n");
		while (out.getch() != '\r') { }
		docalibrate("RP", &controlconfig.roll_pitch_config);
		docalibrate("Y", &controlconfig.yaw_config);
	}
}
static void docalibrate(const char *type, PID::Config *conf) {
	out.printf("Old %s: %f %f %f\n", type, conf->p, conf->i, conf->d);
	out.printf("New %s PID values:\n", type);
	
	static char buf[100];
	out.getline(buf, sizeof(buf));
	
	sscanf(buf, "%f %f %f", &conf->p, &conf->i, &conf->d);
	out.printf("Got: %f %f %f\n", conf->p, conf->i, conf->d);
}
		
