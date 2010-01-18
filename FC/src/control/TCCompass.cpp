#include "TCCompass.h"
#include <FC/math/trig.h>
#include <FC/math/matrix.h>
#include <cmath>

using namespace stmos;
using namespace FC;

TCCompass::TCCompass(const AnalogSensors::Channels &chans, const AnalogSensors::AccelerometerCalibration &cal, stmos::ADC &adc, MicroMag &mmag)
: chans(chans),
  cal(cal),
  adc(adc),
  mmag(mmag),
  heading(0) {
	mmag.startScan(MicroMag::PERIOD_1024, *this);  
} 

void TCCompass::call() {
	MicroMag::Scan scan = mmag.getScan();
	
	ADC::Sample accel_samples[3]; // grab an accelerometer reading
	adc.setSampleChannels(&chans.x_accel, 3);
	adc.sampleMultiple(accel_samples, 3);
	
	float accelvec[3] = { cal.x.getValue(accel_samples[0]), cal.y.getValue(accel_samples[1]), cal.z.getValue(accel_samples[2]) };
	static const float zaccesvec[3] = {0, 0, -1};
	float compaxis[3];
	float compangle = angle_between_vec3(accelvec, zaccesvec, false, compaxis);
	
	static float compmatrix[3*3];
	axis_angle_to_mat(compaxis, compangle, compmatrix);
	
	float magvec[3] = {(float)scan.x, (float)scan.y, (float)scan.z};
	float northvec[3];
	mat::multiply(compmatrix, magvec, 3, 3, 1, northvec);
	
	float relheading = atan2f(northvec[1], northvec[0]);
	
	/*while (heading - relheading > M_PI)
		relheading += M_PI*2;
	while (heading - relheading < -M_PI)
		relheading -= M_PI*2;*/
		
	heading = relheading;
}
