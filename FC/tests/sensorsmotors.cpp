#include <FC/control/Motors.h>
#include <FC/control/Logger.h>
#include <FC/control/TCCompass.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/EEPROM.h>
#include <FC/drivers/MicroMag.h>
#include <FC/drivers/Buzzer.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
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

const AnalogSensors::Channels chans = { {6, 5, 8, 7, 9, 11} };
AnalogSensors::Calibrations calibrations = { {
	{ { 0, 2206 }, // roll
	  { 0, -2206 }, // pitch
	  { 0, -4000 } }, // yaw
	{ { 32000, -6750 }, // x-axis
	  { 32000, -6750 }, // y-axis
	  { 32000, 6750 } } // z-axis
} };	
ADC adc(1);
AnalogSensors analog(adc, chans, calibrations);

const IOPin::PortPin reset = { IOPin::PORT_C, 5 };
const IOPin::PortPin drdy = { IOPin::PORT_B, 10 };
MicroMag mag(2, reset, drdy);

I2C i2c(1);
EEPROM eeprom(i2c, 0x50);
Logger log(eeprom, 1);

Buzzer buzzer;

static void motortaskfunc();
static void logtaskfunc();

CallbackWrapper motorcallback(motortaskfunc);
CallbackWrapper logcallback(logtaskfunc);

Task motortask("motor", Task::PRIORITY_MID, motorcallback, 512);
Task logtask("log", Task::PRIORITY_MID-1, logcallback, 512);

#define LOGSTRUCT_MAGIC 0xAC

struct LogStruct {
	uint8_t magic;
	uint16_t tick;
	float x_accel, z_accel, roll_gyro;
} __attribute__((packed));

LogReader reader(eeprom, 1);

int main(int argc, char **argv) {
	mag.startScan();
	
	out.print("=== Begin dump of previous log ===\n");
	int got;
	LogStruct ls;
	while ((got = reader.read((uint8_t *)&ls, sizeof(ls))) == sizeof(ls)) {
		if (ls.magic != LOGSTRUCT_MAGIC) {
			out.print("Bad magic value\n");
			break;
		}
		out.printf("%u %2.2f %2.2f %2.2f\n", 
		           (unsigned int)ls.tick, ls.roll_gyro, 
		           ls.x_accel, ls.z_accel);
	}
	out.print("=== End dump ===\n");
	
	motortask.start();
	logtask.start();
}

static void motortaskfunc() {
	while (!vexrc.getSynced()) { Task::sleep(100); }

	motors.arm();

	while (true) {
		Task::sleep(50);
		
		if (vexrc.getSynced()) {
			VexRC::Channels chans = vexrc.getChannels();

			float yawcorrection = (chans.analogs[0] / 50.0) / 6;
			float throttle = chans.analogs[1] / 50.0;
			float pitchcorrection = (chans.analogs[2] / 50.0) / 6;
			float rollcorrection = (-chans.analogs[3] / 50.0) / 6;
			
			motors.setThrottle(throttle, rollcorrection, pitchcorrection, yawcorrection);
		} else {
			motors.off();
		}
	}
}

static void logtaskfunc() {
	while (!vexrc.getSynced()) { Task::sleep(100); }
	Task::sleep(4000);
	
	out.print("Begin logging\n");
	calibrations.gyro.roll.center = adc.sampleChannel(chans.roll_gyro);
	calibrations.gyro.pitch.center = adc.sampleChannel(chans.pitch_gyro);
	calibrations.gyro.yaw.center = adc.sampleChannel(chans.yaw_gyro);
	
	while (true) {
		Task::sleep(5);
		
		LogStruct ls;
		ls.magic = LOGSTRUCT_MAGIC;
		ls.tick = (uint16_t)Task::getCurrentTick();
		AnalogSensors::Readings readings = analog.getReadings();
		ls.x_accel = readings.x_accel;
		ls.z_accel = readings.z_accel;
		ls.roll_gyro = readings.roll_gyro;
		
		log.write((uint8_t *)&ls, sizeof(ls));
	}
}

