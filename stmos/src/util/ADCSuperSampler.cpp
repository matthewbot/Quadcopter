#include "ADCSuperSampler.h"
#include <cstring>

using namespace stmos;

ADCSuperSampler::ADCSuperSampler(ADC &adc, const ADC::Channel *chans, size_t chancount, int samplecount) 
: adc(adc),
  dma(adc.getScanDMAChannel()),
  chancount(chancount),
  samplecount(samplecount) {
	adc.setScanChannels(chans, chancount);
	dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_VERY_HIGH, 2);
	
	samplebuf = new ADC::Sample[chancount*samplecount];
}

ADCSuperSampler::~ADCSuperSampler() {
	delete[] samplebuf;
}

void ADCSuperSampler::superSample(ADC::Sample *samples) {
	dma.start(samplebuf, adc.getScanDMAAddress(), chancount*samplecount, false);
	adc.startScan();
	while (!dma.done()) { }
	adc.stopScan();
	
	uint32_t totals[chancount];
	memset(totals, '\0', sizeof(totals));
	
	unsigned int i;
	for (i=0; i<chancount*samplecount; i++) {
		totals[i % chancount] += (uint32_t)samplebuf[i];
	}
	
	for (i=0; i<chancount; i++) {
		samples[i] = (ADC::Sample)(totals[i] / samplecount);
	}
}
