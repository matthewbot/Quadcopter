#include "PPMTimer.h"

using namespace FC;
using namespace stmos;

PPMTimer::PPMTimer() : Timer(4) {
	setTickTime(720);
	setOverflow(4000);
	start();
}

