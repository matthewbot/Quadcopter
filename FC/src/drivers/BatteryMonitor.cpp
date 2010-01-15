#include "BatteryMonitor.h"

#define VOLTAGE_SCALE 5.8213

using namespace FC;
using namespace stmos;

BatteryMonitor::BatteryMonitor(ADC &adc, ADC::Channel chan) 
: adc(adc), 
  chan(chan),
  pin(ADC::getChannelPortPin(chan), IOPinConfig::INPUT_ANALOG) {
	adc.setSampleTime(chan, ADC::SAMPLETIME_239);
}

float BatteryMonitor::getVoltage() {
	ADC::Sample sample = adc.sampleChannel(chan);
	
	float volts = (sample / (float)ADC::MAXSAMPLE) * 3.3;
	return volts * VOLTAGE_SCALE;
}

float BatteryMonitor::getCellVoltage() {
	return getVoltage() / 3; // 3S LiPo
}

