#include "Buzzer.h"
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

Buzzer::Buzzer(bool autobuzz)
: pin(IOPin::PORT_C, 12, IOPin::OUTPUT_OPENDRAIN) {
	if (autobuzz) {
		on();
		Task::sleep(500);
	}
	
	off();
}

void Buzzer::buzz(long duration) {
	on();
	Task::sleep(duration);
	off();
}

