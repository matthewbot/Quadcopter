#include <FC/copter/Quadcopter.h>
#include <FC/copter/ManualThrottlePilot.h>

using namespace FC;

Quadcopter copter(new ManualThrottlePilot());

int main(int argc, char **argv) {	
	copter.run();
}

