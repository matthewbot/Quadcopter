#include "ESCTimer.h"

using namespace FC;
using namespace stmos;

ESCTimer::ESCTimer() : Timer(3) {
	setTickTime(720);
	setOverflow(1000);
	start();
}
