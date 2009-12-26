#include "Motors.h"

using namespace FC;

Motors::Motors(stmos::Timer &tim) 
: north(tim, 1),
  east(tim, 2),
  south(tim, 3),
  west(tim, 4) { }
  
void Motors::arm() {
	north.arm();
	east.arm();
	south.arm();
	west.arm();
}

void Motors::setThrottle(float throttle, float rollcorrection, float pitchcorrection, float yawcorrection) {
	north.setThrottle(throttle + pitchcorrection - yawcorrection);
	south.setThrottle(throttle - pitchcorrection - yawcorrection);
	east.setThrottle( throttle + rollcorrection  + yawcorrection);
	west.setThrottle( throttle - rollcorrection  + yawcorrection);
}

void Motors::off() {
	north.off();
	south.off();
	east.off();
	west.off();
}
