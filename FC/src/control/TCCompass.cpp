#include "TCCompass.h"
#include <FC/math/trig.h>
#include <FC/math/matrix.h>
#include <cmath>

using namespace stmos;
using namespace FC;

TCCompass::TCCompass(MicroMag &mmag)
: mmag(mmag),
  prevheading(0) {
} 

float TCCompass::calcHeading(float roll, float pitch) {
	MicroMag::Scan scan = mmag.scan();
	
	float tmp = roll;
	roll = -pitch;
	pitch = -tmp;
	
	float sroll = sinf(roll);
	float croll = cosf(roll);
	float spitch = sinf(pitch);
	float cpitch = cosf(pitch);
	
	float magx = scan.x*cpitch + scan.y*sroll*spitch - scan.z*croll*spitch;
	float magy = scan.y*croll + scan.z*sroll;

	float relheading = atan2f(magy, magx);
	
	/*while (heading - relheading > M_PI)
		relheading += M_PI*2;
	while (heading - relheading < -M_PI)
		relheading -= M_PI*2;*/
		
	prevheading = relheading;
	return prevheading;
}
