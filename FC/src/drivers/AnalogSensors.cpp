#include "AnalogSensors.h"
#include <stmos/util/Task.h>
#include <stmos/peripherals/IOPin.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(ADC &adc, const Channels &channels, const Calibrations &calibrations)
: calibrations(calibrations),
  adc(adc),
  adc_dma(adc.getScanDMAChannel()) {
	int i;
	for (i=0;i<6;i++)
		IOPin::setup(ADC::getChannelPortPin(channels.array[i]), IOPin::INPUT_ANALOG);
		
	adc.setScanChannels(channels.array, 6);  
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
}

AnalogSensors::Readings AnalogSensors::getReadings() {
	ADC::Sample samples[6];
	adc_dma.start(samples, adc.getScanDMAAddress(), 6, false);
	adc.singleScan();
	adc_dma.wait();
	
	Readings readings;
	int i;
	for (i=0; i<6; i++) {
		readings.array[i] = calibrations.array[i].getValue(samples[i]);
	}
	
	return readings;
}

float AnalogSensors::Calibration::getValue(stmos::ADC::Sample in) const {
	return (in - center) * scale;
}

