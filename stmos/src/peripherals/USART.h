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
			~USART();
			
			// string IO
			void print(const char *msg);
			void printf(const char *fmt, ...);
			char getch(); // blocking
			size_t getline(char *buf, size_t bufsize); // blocking
			
			// binary IO
			void send(const uint8_t *buf, size_t len);
			int receive(uint8_t *buf, size_t maxlen); // nonblocking
			
			void irq();
		private:
			const int num;
			char *printfbuf;

			char recvbuf[16];
			uint8_t recvbuf_pos;
			
			IOPinConfig txpin, rxpin;
	};
}

#endif
