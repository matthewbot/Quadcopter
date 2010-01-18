#include <FC/drivers/MicroMag.h>
#include <stmos/peripherals/USART.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

MicroMag mag(2, (IOPin::PortPin) { IOPin::PORT_C, 5 }, (IOPin::PortPin)  { IOPin::PORT_B, 10 });
USART out(1, 115200);

int main(int argc, char **argv) {	
	out.print("Starting micromag scan\n");
	
	mag.startScan();
	
	while (true) {
		Task::sleep(500);
		MicroMag::Scan scan = mag.getScan();
		
		out.printf("%d %d %d\n", (int)scan.x, (int)scan.y, (int)scan.z);
	}
}
