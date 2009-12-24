#include "Motors.h"

using namespace FC;

Motors::Motors(int timernum) 
: tim(timernum),
  north(tim, 1),
  east(tim, 2),
  south(tim, 3),
  west(tim, 4) {
	tim.setTickTime(720);
	tim.setOverflow(1000);
	tim.start();  
}
  
void Motors::arm(Motor mot) {
	getESC(mot).arm();
}
  
void Motors::armAll() {
	north.arm();
	east.arm();
	south.arm();
	west.arm();
}

void Motors::setThrottle(Motor mot, float throttle) {
	getESC(mot).setThrottle(throttle);
}

void Motors::setThrottleAll(float throttle) {
	north.setThrottle(throttle);
	east.setThrottle(throttle);
	south.setThrottle(throttle);
	west.setThrottle(throttle);
}

ESC &Motors::getESC(Motor mot) {
	switch (mot) {
		case NORTH:
			return north;
			
		case EAST:
			return east;
			
		case SOUTH:
			return south;
			
		case WEST:
			return west;
	}
}
