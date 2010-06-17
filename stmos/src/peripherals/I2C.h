#ifndef STMOS_PERIPHERALS_I2C_H
#define STMOS_PERIPHERALS_I2C_H

#include <stmos/peripherals/IOPin.h>
#include <stmos/util/NonCopyable.h>
#include <stdint.h>
#include <cstddef>

namespace stmos {
	class I2C : NonCopyable {
		public:
			typedef uint8_t Address;
		
			I2C(int num);
			void reset();
			
			void start(Address addr);
			void send(const uint8_t *buf, size_t size);
			void stop();
			
			void receive(Address addr, uint8_t *data, size_t size);
		
		private:
			int num;
			
			IOPinConfig scl_pin, sda_pin;
	};
}

#endif
