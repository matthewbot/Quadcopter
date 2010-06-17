#ifndef STMOS_PERIPHERALS_I2C_H
#define STMOS_PERIPHERALS_I2C_H

#include <stmos/peripherals/IOPin.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Mutex.h>
#include <stmos/util/Notifier.h>
#include <stdint.h>
#include <cstddef>

namespace stmos {
	class I2C : IRQCallback, NonCopyable {
		public:
			typedef uint8_t Address;
		
			enum Mode {
				SEND,
				RECEIVE
			};
		
			I2C(int num);
			void reset();
			
			void start(Address addr, Mode mode);
			void send(const uint8_t *buf, size_t size);
			void stop();
			
			void receive(uint8_t *data, size_t size, bool nack);
		
			void irq();
			virtual void irqcallback();
		private:
			int num;
			
			Mutex mutex;
			Notifier notifier;
			
			Address curaddress;
			union {
				const uint8_t *cursendbuf;
				uint8_t *currecvbuf;
			};
			size_t curbufsize;
			bool curnack;
			
			IOPinConfig scl_pin, sda_pin;
	};
}

#endif
