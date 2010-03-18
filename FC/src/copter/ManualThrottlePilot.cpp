#include "ManualThrottlePilot.h"
#include <cmath>

using namespace FC;

ManualThrottlePilot::ManualThrottlePilot() : heading(0) { }
ManualThrottlePilot::~ManualThrottlePilot() { }

void ManualThrottlePilot::start(Quadcopter &quad) {
	heading = quad.getIMU().getYaw();
}

void ManualThrottlePilot::run(Quadcopter &quad) { 
	VexRC &vexrc = quad.getVexRC();
	
	if (!vexrc.getSynced())
		return;
		
	VexRC::Channels chans = vexrc.getChannels();
		
	float throttle = chans.analogs[1] / 50.0;
	if (throttle < 0)
		throttle = 0;
	float rollsetpoint = (-chans.analogs[3] / 50.0) * 0.3;
	float pitchsetpoint = (chans.analogs[2] / 50.0) * 0.3;
	heading += (chans.analogs[0] / 50.0) * (M_PI / 5) * CYCLETIME;

	quad.getMotorsController().setControlPoints(throttle, rollsetpoint, pitchsetpoint, heading);
}
