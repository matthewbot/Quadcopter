#include "AnalogSensors.h"
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

AnalogSensors::AnalogSensors(ADC &adc, const Channels &channels, const Calibrations &calibrations)
: calibrations(calibrations),
  adc(adc),
  adc_dma(adc.getScanDMAChannel()),
  roll_pin(ADC::getChannelPortPin(channels.roll_gyro), IOPin::INPUT_ANALOG),
  pitch_pin(ADC::getChannelPortPin(channels.pitch_gyro), IOPin::INPUT_ANALOG),
  yaw_pin(ADC::getChannelPortPin(channels.yaw_gyro), IOPin::INPUT_ANALOG),
  accel_x_pin(ADC::getChannelPortPin(channels.x_accel), IOPin::INPUT_ANALOG),
  accel_y_pin(ADC::getChannelPortPin(channels.y_accel), IOPin::INPUT_ANALOG),
  accel_z_pin(ADC::getChannelPortPin(channels.z_accel), IOPin::INPUT_ANALOG) {
	adc.setScanChannels(channels.array, 6);  
	adc_dma.setup(DMA::DIRECTION_PER_TO_MEM, DMA::PRIORITY_LOW, sizeof(ADC::Sample));
}

#include <stmos/crt/debug.h>

AnalogSensors::Readings AnalogSensors::getReadings() {
	ADC::Sample samples[6];
	adc_dma.start(samples, adc.getScanDMAAddress(), 6, false);
	adc.startScan();
	adc_dma.wait();
	adc.stopScan();
	
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

