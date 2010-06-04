#include "AnalogSensors.h"
#include <stmos/util/Task.h>
#include <stmos/peripherals/IOPin.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(ADC &adc, const Channels &channels, const Calibrations &calibrations, const float *alphas)
: calibrations(calibrations),
  adc(adc),
  adc_dma(adc.getScanDMAChannel()),
  task("analog", Task::PRIORITY_HIGH+10, *this) {
	int i;
	for (i=0;i<6;i++)
		IOPin::setup(ADC::getChannelPortPin(channels.array[i]), IOPin::INPUT_ANALOG);
	
	for (i=0;i<6;i++)
		filters[i].setAlpha(alphas[i]);
	
	adc.setScanChannels(channels.array, 6);  
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
	task.start();
}

AnalogSensors::Readings AnalogSensors::getReadings() {
	Readings readings;
	
	int i;
	for (i=0;i<6;i++)
		readings.array[i] = filters[i].getValue();
		
	return readings;
}

void AnalogSensors::call() {
	while (true) {
		ADC::Sample samples[6];		
		adc_dma.start(samples, adc.getScanDMAAddress(), 6, false);
		adc.singleScan();
		adc_dma.wait();
		
		int i;
		for (i=0;i<6;i++) {
			float val = calibrations.array[i].getValue(samples[i]);
			filters[i].filter(val);
		}
		
		Task::sleep(1);
	}
}

float AnalogSensors::Calibration::getValue(stmos::ADC::Sample in) const {
	return (in - center) * scale;
}

