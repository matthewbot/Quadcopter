#include <FC/drivers/AnalogSampler.h>
#include <FC/util/configs.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>
#include <cmath>

using namespace FC;
using namespace stmos;

ADC adc(1);
AnalogSampler sampler(adc, configs::analogchannels);
AnalogSensors analog(sampler, configs::analogconfig);
USART out(1, 115200);

inline float sqr(float x) { return x*x; }

int main(int argc, char **argv) {
	while (true) {		
		AnalogSensors::Readings readings = analog.getReadings();
		
		int i;
		for (i=0; i<6; i++)
			out.printf("%.3f ", readings.array[i]);
		out.print("\n");
		
		Task::sleep(5);
	}
}
