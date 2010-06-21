#include "AnalogSampler.h"
#include <stmos/util/Task.h>
#include <stmos/peripherals/IOPin.h>
#include <string.h>

using namespace FC;
using namespace stmos;

AnalogSampler::AnalogSampler(ADC &adc, const Channels &channels)
: adc(adc),
  adc_dma(adc.getScanDMAChannel()) {
	int i; 
	for (i=0;i<SENSOR_COUNT;i++)
		IOPin::setup(ADC::getChannelPortPin(channels.array[i]), IOPin::INPUT_ANALOG);
		
	adc.setScanChannels(channels.array, SENSOR_COUNT);
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
}

AnalogSampler::Samples AnalogSampler::getSamples() {	
	Samples samples;
	adc_dma.start(samples.array, adc.getScanDMAAddress(), SENSOR_COUNT, false);
	adc.singleScan();
	adc_dma.wait();
	return samples;
}

AnalogSampler::Samples AnalogSampler::getAveragedSamples(int samples, int tdelta) {
	int totals[SENSOR_COUNT];
	int i;
	
	for (i=0; i<SENSOR_COUNT; i++)
		totals[i] = 0;
	
	for (i=0; i<samples; i++) {
		Samples samples = getSamples();
		int j;
		for (j=0;j<SENSOR_COUNT; j++)
			totals[j] += samples.array[j];
			
		Task::sleep(tdelta);
	}
	
	Samples ret;
	for (i=0; i<SENSOR_COUNT; i++)
		ret.array[i] = totals[i] / samples;
	return ret;
}

