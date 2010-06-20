#include "PPMTimer.h"

using namespace FC;
using namespace stmos;

PPMTimer::PPMTimer() : Timer(4, IRQ_PRIORITY_MED) {
	setTickTime(72);
	setOverflow(40000);
	start();
}

