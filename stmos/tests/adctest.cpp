#include <stmos/peripherals/USART.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/DMA.h>
#include <stmos/util/Task.h>

using namespace stmos;

USART out(1, 115200);
ADC adc(1);
DMAWait adc_dma(adc.getScanDMAChannel());

static const ADC::Channel channels[] = { 7, 9, 11 };

int main(int argc, char **argv) {	
	out.print("Hello World!\n");
	
	int i;
	for (i=0; i<3; i++) {
		IOPin::setup(ADC::getChannelPortPin(channels[i]), IOPin::INPUT_ANALOG, IOPin::NONE, false);
		adc.setSampleTime(channels[i], ADC::SAMPLETIME_239);
	}
	
	ADC::Sample buf[3];
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
	adc.setScanChannels(channels, 3);
	adc.setSampleChannels(channels, 3);
	
	while (true) {
		Task::sleep(500);
		out.print("Scan: ");
		adc_dma.start(buf, adc.getScanDMAAddress(), 3, false);
		adc.singleScan();
		adc_dma.wait();
		out.printf("%d %d %d!\n", buf[0], buf[1], buf[2]);
		
		Task::sleep(500);
		out.print("Sample: ");
		adc.sampleMultiple(buf, 3);
		out.printf("%d %d %d!\n", buf[0], buf[1], buf[2]);		
	}
}
