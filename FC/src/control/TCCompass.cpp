#include "TCCompass.h"
#include <FC/math/trig.h>
#include <FC/math/matrix.h>
#include <cmath>

using namespace stmos;
using namespace FC;

TCCompass::TCCompass(MicroMag &mmag)
: mmag(mmag),
  prevheading(0) {
	mmag.startScan(MicroMag::PERIOD_1024);  
} 

float TCCompass::calcHeading(float roll, float pitch) {
	MicroMag::Scan scan = mmag.getScan();
	
	float magx = scan.x*cos(pitch) + scan.y*sin(roll)*sin(pitch) - scan.z*cos(roll)*sin(pitch);
	float magy = scan.y*cos(roll) + scan.z*sin(roll);

	float relheading = atan2f(magy, magx);
	
	/*while (heading - relheading > M_PI)
		relheading += M_PI*2;
	while (heading - relheading < -M_PI)
		relheading -= M_PI*2;*/
		
	prevheading = relheading;
	return prevheading;
}
