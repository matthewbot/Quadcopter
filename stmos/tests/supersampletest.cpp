#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/util/ADCSuperSampler.h>
#include <stmos/util/Task.h>

using namespace stmos;

const ADC::Channel chans[] = {7, 9, 11};
#define CHANCOUNT sizeof(chans)/sizeof(ADC::Channel)

ADCSuperSampler sampler(1, chans, CHANCOUNT, 50);
USART out(1, 115200);
IOPin x_acc(IOPin::PORT_B, 1, IOPin::INPUT_ANALOG);
IOPin y_acc(IOPin::PORT_A, 5, IOPin::INPUT_ANALOG);
IOPin z_acc(IOPin::PORT_C, 1, IOPin::INPUT_ANALOG);

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
	
	
	while (true) {
		Task::sleep(500);
		
		ADC::Sample samples[CHANCOUNT];
		sampler.superSample(samples);
		
		unsigned int i;
		for (i=0; i<CHANCOUNT; i++) {
			out.printf("%d ", (int)samples[i]);
		}
		
		out.print("\n");
	}
}
