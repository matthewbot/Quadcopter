#include "AnalogSensors.h"
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(ADC &adc, const Channels &channels, const Calibrations &calibrations)
: calibrations(calibrations),
  adc(adc),
  sampler(adc, channels.array, 6, 10),
  roll_pin(ADC::getChannelPortPin(channels.roll_gyro), IOPin::INPUT_ANALOG),
  pitch_pin(ADC::getChannelPortPin(channels.pitch_gyro), IOPin::INPUT_ANALOG),
  yaw_pin(ADC::getChannelPortPin(channels.yaw_gyro), IOPin::INPUT_ANALOG),
  accel_x_pin(ADC::getChannelPortPin(channels.x_accel), IOPin::INPUT_ANALOG),
  accel_y_pin(ADC::getChannelPortPin(channels.y_accel), IOPin::INPUT_ANALOG),
  accel_z_pin(ADC::getChannelPortPin(channels.z_accel), IOPin::INPUT_ANALOG) { }

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

