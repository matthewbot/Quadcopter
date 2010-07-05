#include "ESCTimer.h"

using namespace FC;
using namespace stmos;

ESCTimer::ESCTimer() : Timer(3, IRQ_PRIORITY_MED+1) {
	setTickTime(72);
	setOverflow(20000);
	start();
}
