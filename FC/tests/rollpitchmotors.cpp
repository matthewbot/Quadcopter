#include <FC/control/Motors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/MicroMag.h>
#include <FC/drivers/Buzzer.h>
#include <FC/drivers/BatteryMonitor.h>
#include <FC/control/TCCompass.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

USART out(1, 115200);
PPMTimer ppmtim;
VexRC vexrc(ppmtim, 4);

ESCTimer esctim;
Motors motors(esctim);

ADC adc(1);
AnalogSensors::Calibrations calibrations = configs::analog;
AnalogSensors analog(adc, configs::chans, calibrations, configs::alphas);

Buzzer buzzer;
BatteryMonitor batmon(adc, 13);

int main(int argc, char **argv) {
	out.printf("Battery: %f cell\n", batmon.getCellVoltage());
	out.print("Waiting for vexrc\n");
	while (!vexrc.getSynced()) { Task::sleep(100); }

	out.print("Arming\n");
	motors.arm();

	out.print("Ready!\n");
	while (true) {
		long start = Task::getCurrentTick();
		
		if (vexrc.getSynced()) {
			VexRC::Channels chans = vexrc.getChannels();

			float yawcorrection = (chans.analogs[0] / 50.0) / 6;
			float throttle = chans.analogs[1] / 50.0;
			float pitchcorrection = (chans.analogs[2] / 50.0) / 6;
			float rollcorrection = (-chans.analogs[3] / 50.0) / 6;
			
			motors.setThrottle(throttle, rollcorrection, pitchcorrection, yawcorrection);
			
			AnalogSensors::Readings readings = analog.getReadings();
			out.printf("%f %f %f %f %f %f\n", readings.roll_gyro, readings.pitch_gyro, readings.yaw_gyro, readings.x_accel, readings.y_accel, readings.z_accel);
		
		} else {
			motors.off();
		}
		
		Task::sleep(5 - (Task::getCurrentTick() - start));
	}
}

