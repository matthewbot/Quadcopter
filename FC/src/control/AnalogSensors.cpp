#include "AnalogSensors.h"
#include <stmos/util/Task.h>
#include <stmos/peripherals/IOPin.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(AnalogSampler &sampler, const Config &config)
: config(config),
  sampler(sampler),
  task("analog", Task::PRIORITY_HIGH, *this) {
	int i;
	for (i=0;i<6;i++)
		filters[i].setAlpha(config.alphas[i]);
	
	for (i=0;i<3;i++)
		sensorcenters[i+3] = config.accelcenters[i];
	
	centerGyros();
	task.start();
}

void AnalogSensors::centerGyros() {
	AnalogSampler::Samples samples = sampler.getAveragedSamples(1000, 1);
	int i;
	for (i=0;i<3;i++)
		sensorcenters[i] = samples.array[i];
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
		AnalogSampler::Samples samples = sampler.getSamples();
		
		int i;
		for (i=0;i<6;i++) {
			float val = (samples.array[i] - sensorcenters[i]) * config.scales[i];
			filters[i].filter(val);
		}
		
		Task::sleep(1);
	}
}

