#include <FC/drivers/MicroMag.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

static MicroMag mag(2, IOPin::PORT_C, 5, IOPin::PORT_B, 10);
static USART out(1, 115200);

int main(int argc, char **argv) {	
	out.printf("Single axis X: %d\n", (int)mag.sample(MicroMag::AXIS_X));
	out.print("Starting scan\n");
	
	mag.startScan();
	
	while (true) {
		Task::sleep(500);
		MicroMag::Scan scan = mag.getScan();
		
		out.printf("%d %d %d\n", (int)scan.x, (int)scan.y, (int)scan.z);
	}
}
