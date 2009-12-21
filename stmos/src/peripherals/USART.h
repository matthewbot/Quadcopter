#ifndef STMOS_PERIPHERALS_USART_H
#define STMOS_PERIPHERALS_USART_H

#include "IOPin.h"
#include <stmos/util/NonCopyable.h>
#include <stdint.h>
#include <cstddef>

namespace stmos {
	class USART : NonCopyable {
		public:
			USART(int num, unsigned int baud);
			
			// string IO
			void print(const char *msg) const;
			void printf(const char *fmt, ...) const;
			
			// binary IO
			void send(const uint8_t *buf, size_t len) const;
			void receive(uint8_t *buf, size_t len) const;
		private:
			const int num;
			mutable char *printfbuf;
			IOPinConfig txpin, rxpin;
	};
}

#endif
