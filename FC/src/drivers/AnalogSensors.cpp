#include "AnalogSensors.h"
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(int adcnum, const Channels &channels, const Calibrations &calibrations)
: calibrations(calibrations),
  adc(adcnum),
  sampler(adc, channels.array, 6, 10) { }
  
AnalogSensors::Readings AnalogSensors::getReadings() {
	ADC::Sample samples[6];
	sampler.superSample(samples);
	
	Readings readings;
	int i;
	for (i=0; i<6; i++) {
		readings.array[i] = calibrations.array[i].getValue(samples[i]);
	}
	
	return readings;
}

float AnalogSensors::Calibration::getValue(stmos::ADC::Sample in) const {
	return (in - center) / scale;
}

