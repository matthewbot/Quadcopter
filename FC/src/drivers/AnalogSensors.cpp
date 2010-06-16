#include "AnalogSensors.h"
#include <stmos/util/Task.h>
#include <stmos/peripherals/IOPin.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(ADC &adc, const Config &config)
: config(config),
  adc(adc),
  adc_dma(adc.getScanDMAChannel()),
  task("analog", Task::PRIORITY_HIGH+10, *this) {
	int i;
	for (i=0;i<6;i++)
		IOPin::setup(ADC::getChannelPortPin(config.channels.array[i]), IOPin::INPUT_ANALOG);
	
	for (i=0;i<6;i++)
		filters[i].setAlpha(config.alphas[i]);

	for (i=0;i<6;i++)
		sensorcenters[i] = 32768;
	
	adc.setScanChannels(config.channels.array, 6);  
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
	centerGyros();
	task.start();
}

void AnalogSensors::centerGyros() {
	int totsamples[3] = {0, 0, 0};
	int i;
	for (i=0;i<1000;i++) {
		ADC::Sample samples[6];
		getSamples(samples);
		int j;
		for (j=0;j<3;j++) {
			totsamples[j] += samples[j];
		}
		Task::sleep(1);
	}
	
	for (i=0;i<3;i++) {
		sensorcenters[i] = totsamples[i]/1000;
	}
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
		getSamples(samples);
		
		int i;
		for (i=0;i<6;i++) {
			float val = config.calibrations.array[i].getValue(samples[i], sensorcenters[i]);
			filters[i].filter(val);
		}
		
		Task::sleep(1);
	}
}

void AnalogSensors::getSamples(ADC::Sample *samples) {	
	adc_dma.start(samples, adc.getScanDMAAddress(), 6, false);
	adc.singleScan();
	adc_dma.wait();
}

float AnalogSensors::Calibration::getValue(stmos::ADC::Sample in, stmos::ADC::Sample center) const {
	return (in - center) * scale;
}

