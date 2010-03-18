#include "Quadcopter.h"
#include <FC/copter/Pilot.h>
#include <stmos/util/Task.h>

using namespace FC;
using namespace stmos;

Quadcopter::Quadcopter(Pilot *initialpilot) : pilot(initialpilot), cyclecount(0) { }
Quadcopter::~Quadcopter() { delete pilot; }

void Quadcopter::run() {
	start();
	
	while (true) {
		long start = Task::getCurrentTick();
		
		if (!vexrc.getSynced()) {
			out.print("Lost radio!\n");
			break;
		}
		
		if (cyclecount % 1000 == 0 && cyclecount > 0) 
			out.printf("Battery level: %f\n", batmon.getVoltage());
		
		pilot->run(*this);
		cyclecount++;
		Task::sleep((long)(Pilot::CYCLETIME*1000) - Task::getCurrentTick() - start);
	}
	
	stop();
}

void Quadcopter::start() {
	out.printf("Battery level: %f\n", batmon.getVoltage());

	out.print("Waiting for IMU\n");
	while (!imu.ready()) { Task::sleep(100); }
	
	out.print("Waiting for VexRC\n");
	while (!vexrc.getSynced()) { Task::sleep(100); }
	
	out.print("Arming!\n");
	buzzer.buzz(1000);
	Task::sleep(500);
	
	motors.arm();
	motcontrol.start();
	
	out.print("Ready!\n");
	buzzer.buzz(200);
}

void Quadcopter::stop() {
	motcontrol.stop();
	motors.off();
	out.print("Stopping motors\n");
	buzzer.buzz(400);
}

