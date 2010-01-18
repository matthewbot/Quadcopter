#include "ESCTimer.h"

using namespace FC;
using namespace stmos;

ESCTimer::ESCTimer() : Timer(3) {
	setTickTime(72);
	setOverflow(20000);
	start();
}
