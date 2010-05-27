#ifndef STMOS_PERIPHERALS_USART_H
#define STMOS_PERIPHERALS_USART_H

#include "IOPin.h"
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Notifier.h>
#include <stmos/util/Mutex.h>
#include <stmos/util/IRQCallback.h>
#include <stdint.h>
#include <cstddef>

namespace stmos {
	class USART : IRQCallback, NonCopyable {
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
			virtual void irqcallback();
		private:
			const int num;
			char *printfbuf;

			uint8_t recvbuf[16];
			uint8_t recvbuf_pos;
			
			const uint8_t *sendbuf;
			size_t sendbuf_len;
			
			Mutex mutex;
			Notifier notifier;
			
			IOPinConfig txpin, rxpin;
	};
}

#endif
