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
	imu.start();
	
	while (true) {
		Task::sleep(20);
		
		IMU::State state = imu.getState();
		out.printf("%0.3f %0.3f %0.3f\n", state.roll, state.pitch, state.yaw);
	}
}

