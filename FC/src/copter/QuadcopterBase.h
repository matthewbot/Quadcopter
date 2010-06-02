#ifndef FC_COPTER_QUADCOPTERBASE_H
#define FC_COPTER_QUADCOPTERBASE_H

// Base struct just contains and properly initializes all quadcopter subsystems

#include <FC/control/IMU.h>
#include <FC/control/Motors.h>
#include <FC/control/MotorsController.h>
#include <FC/drivers/AnalogSensors.h>
#include <FC/drivers/VexRC.h>
#include <FC/drivers/Buzzer.h>
#include <FC/drivers/BatteryMonitor.h>
#include <FC/util/ESCTimer.h>
#include <FC/util/PPMTimer.h>
#include <stmos/peripherals/IOPin.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/USART.h>

namespace FC {
	struct QuadcopterBase {
		QuadcopterBase();
		
		stmos::USART out;
		
		stmos::ADC adc;
		stmos::ADC adc2;
		AnalogSensors analog;
		
		IMU imu;
		
		ESCTimer esctim;
		Motors motors;
		
		MotorsController::Config controlconfig;
		MotorsController motcontrol;
		
		PPMTimer ppmtim;
		VexRC vexrc;
		
		Buzzer buzzer;
		BatteryMonitor batmon;
	};
}


#endif
