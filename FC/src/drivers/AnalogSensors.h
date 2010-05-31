#ifndef FC_DRIVERS_ANALOGSENSORS_H
#define FC_DRIVERS_ANALOGSENSORS_H

#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/DMA.h>
#include <stmos/peripherals/IOPin.h>

namespace FC {
	class AnalogSensors {
		public:
			union Channels {
				struct {
					stmos::ADC::Channel roll_gyro, pitch_gyro, yaw_gyro;
					stmos::ADC::Channel x_accel, y_accel, z_accel;
				};
	
				stmos::ADC::Channel array[6];
			};
	
			struct Calibration {
				stmos::ADC::Sample center;
				float scale;
				
				float getValue(stmos::ADC::Sample in) const;
			} __attribute__((packed));
	
			struct AccelerometerCalibration {
				Calibration x, y, z;
			} __attribute__((packed));
			
			struct GyroCalibration {
				Calibration roll, pitch, yaw;
			} __attribute__((packed));
	
			union Calibrations {
				struct {
					GyroCalibration gyro;
					AccelerometerCalibration accel;
				} __attribute__((packed));
		
				Calibration array[6];
			};
		
			union Readings {
				struct {
					float roll_gyro, pitch_gyro, yaw_gyro;
					float x_accel, y_accel, z_accel;
				};
				
				float array[6];
			};
		
			AnalogSensors(stmos::ADC &adc, const Channels &channels, const Calibrations &calibrations);
	
			Readings getReadings();
			
		private:
			const Calibrations &calibrations;
			stmos::ADC &adc;
			stmos::DMAWait adc_dma;
			stmos::IOPinConfig roll_pin, pitch_pin, yaw_pin, accel_x_pin, accel_y_pin, accel_z_pin;
	};
}

#endif
