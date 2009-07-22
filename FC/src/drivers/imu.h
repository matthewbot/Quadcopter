#ifndef FC_DRIVERS_IMU_H
#define FC_DRIVERS_IMU_H

#include <stdint.h>

struct imu_rpy {
	float roll, pitch, yaw;
};

struct imu_data {
	struct imu_rpy gyro;
	struct imu_rpy accel;
};

void imu_init();
const uint16_t *imu_read_raw();
struct imu_data imu_read();

#endif
