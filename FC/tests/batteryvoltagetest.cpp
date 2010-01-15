#include <FC/drivers/BatteryMonitor.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace stmos;
using namespace FC;

USART out(1, 115200);
ADC adc(1);
BatteryMonitor batmon(adc, 13);

int main(int argc, char **argv) {	
	while (true) {
		out.printf("%f\n", batmon.getVoltage());
		Task::sleep(500);
	}
}

