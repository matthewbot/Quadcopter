#include <FC/control/Motors.h>
#include <FC/control/IMU.h>
#include <FC/control/MotorsController.h>
#include <FC/control/Logger.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/drivers/BatteryMonitor.h>
#include <FC/drivers/EEPROM.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>
#include <cstdio>
#include <cmath>

using namespace FC;
using namespace stmos;

USART out(1, 115200);

ADC adc(1);
AnalogSensors sensors(adc, configs::analog);

IMU imu(sensors, configs::imu);

ESCTimer esctim;
Motors motors(esctim);

MotorsController::Config controlconfig = FC::configs::control;
MotorsController control(controlconfig, imu, motors, esctim);

PPMTimer ppmtim;
VexRC vex(ppmtim, 4);

I2C i2c(1);
EEPROM eeprom(i2c, 0x50);

Buzzer buzzer;
BatteryMonitor batmon(adc, 13);

static void docalibrate(const char *type, PID::Config *pid);

struct LogEntry {
	AnalogSensors::Readings analogs;
	IMU::State state;
	float throttle;
};

int main(int argc, char **argv) {
	imu.start();

	out.printf("Battery: %f cell\n", batmon.getCellVoltage());
	out.print("Waiting for remote signal\n");
	while (!vex.getSynced()) { Task::sleep(100); }

	out.print("Arming\n");
	motors.arm();

	float heading = imu.getYaw();
	buzzer.buzz(300);

	while (true) {
		control.start();
		Logger logger(eeprom, 0);
		bool logging = false;
		
		while (true) {
			bool up = false;
			if (!vex.getSynced()) {
				control.stop();
				motors.off();
				while (!vex.getSynced()) { Task::sleep(100); }
				control.start();
				sensors.centerGyros();
				heading = imu.getYaw();
				buzzer.buzz(300);
			}
		
			VexRC::Channels chans = vex.getChannels();
	
			if (chans.right != VexRC::NONE)
				break;
			if (chans.left == VexRC::UP)
				logging = true;
			else if (chans.left == VexRC::DOWN)
				logging = false;
	
			float throttle = chans.analogs[1] / 50.0;
			if (throttle < 0)
				throttle = 0;
			float rollsetpoint = (-chans.analogs[3] / 50.0) * 0.3;
			float pitchsetpoint = (-chans.analogs[2] / 50.0) * 0.3;
			heading += (chans.analogs[0] / 50.0) * (M_PI / 4) * .005;
			
			control.setControlPoints(throttle, rollsetpoint, pitchsetpoint, heading);
			if (!up) {
				if (throttle > 0.4)
					up = true;
				else
					control.clearIntegrals();
			}
			
			if (logging) {
				LogEntry entry = { sensors.getReadings(), imu.getState(), throttle };
				logger.write((uint8_t *)&entry, sizeof(entry));
			}
			
			Task::sleep(5);
		}
		
		control.stop();
		motors.off();
		out.print("Push enter\n");
		while (out.getch() != '\r') { }
		
		out.print("Log dump:");
		LogReader reader(eeprom, 0);
		struct LogEntry entry;
		while (reader.read((uint8_t *)&entry, sizeof(entry)) == sizeof(entry)) {
			int i;
			for (i=0;i<6;i++) {
				out.printf("%.3f ", entry.analogs.array[i]);
			}
			out.printf("%.3f %.3f %.3f ", entry.state.roll, entry.state.pitch, entry.state.yaw);
			out.printf("%.3f\n", entry.throttle);
		}
		
		docalibrate("Roll", &controlconfig.roll_config);
		docalibrate("Pitch", &controlconfig.pitch_config);
		docalibrate("Yaw", &controlconfig.yaw_config);
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
		
