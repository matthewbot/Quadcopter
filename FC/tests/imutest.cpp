#include <FC/control/IMU.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/USART.h>

using namespace FC;
using namespace stmos;

ADC adc(1);
AnalogSensors sensors(adc, configs::chans, configs::analog);

IMU imu(sensors, configs::imu);
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

