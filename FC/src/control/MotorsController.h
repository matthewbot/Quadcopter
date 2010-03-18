#ifndef FC_CONTROL_MOTORSCONTROLLER_H
#define FC_CONTROL_MOTORSCONTROLLER_H

#include <FC/control/Motors.h>
#include <FC/control/IMU.h>
#include <FC/math/PID.h>
#include <FC/util/ESCTimer.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>

namespace FC {
	class MotorsController : stmos::NonCopyable, stmos::Callback {
		public:
			struct Config {
				PID::Config roll_config;
				PID::Config pitch_config;
				PID::Config yaw_config;
			} __attribute__((packed));
		
			MotorsController(const Config &config, IMU &imu, Motors &motors, ESCTimer &esctimer);
			
			void start();
			void stop();
			
			void setControlPoints(float throttle, float roll, float pitch, float yaw);
			
			inline float getRollCorrection() const { return roll_correction; }
			inline float getPitchCorrection() const { return pitch_correction; }
			inline float getYawCorrection() const { return yaw_correction; }
			
		//private:
			IMU &imu;
			Motors &motors;
			
			bool running;
			PID roll_pid, pitch_pid, yaw_pid;
			float throttle;
			float roll_setpoint, pitch_setpoint, yaw_setpoint;
			float roll_correction, pitch_correction, yaw_correction;
			virtual void call();
	};

}

#endif
