#include <FC/drivers/AnalogSensors.h>
#include <FC/util/configs.h>
#include <FC/math/trig.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>
#include <cmath>

using namespace FC;
using namespace stmos;

ADC adc(1);
AnalogSensors analog(adc, configs::chans, configs::analog, configs::alphas);
USART out(1, 115200);

int main(int argc, char **argv) {	
	while (true) {
		Task::sleep(100);
		AnalogSensors::Readings readings = analog.getReadings();
		
		int i;
		for (i=0; i<6; i++)
			out.printf("%0.3f ", readings.array[i]);
		out.print("\n");
	}
}
