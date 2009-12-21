#ifndef STMOS_PERIPHERALS_I2C_H
#define STMOS_PERIPHERALS_I2C_H

#include <stmos/util/NonCopyable.h>
#include <stdint.h>
#include <cstddef>

namespace stmos {
	class I2C : NonCopyable {
		public:
			I2C(int num);
			
			void start(int addr);
			void send(const uint8_t *buf, size_t size);
			void stop();
			
			void receive(int addr, uint8_t *data, size_t size);
		
		private:
			int num;
	
	};
}

#endif
