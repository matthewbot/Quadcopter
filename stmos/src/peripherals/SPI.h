#ifndef STMOS_PERIPHERALS_SPI_H
#define STMOS_PERIPHERALS_SPI_H

#include "IOPin.h"
#include <stmos/util/NonCopyable.h>
#include <stmos/util/IRQCallback.h>
#include <stmos/util/Mutex.h>
#include <stmos/util/Notifier.h>
#include <cstddef>

namespace stmos {
	class SPI : IRQCallback, NonCopyable {
		public:
			enum Baud {
				BAUDDIV_2,
				BAUDDIV_4,
				BAUDDIV_8,
				BAUDDIV_16,
				BAUDDIV_32,
				BAUDDIV_64,
				BAUDDIV_128,
				BAUDDIV_256	
			};
		
			SPI(int num, Baud baud);
			~SPI();

			void sendReceive(const uint8_t *sendbuf, uint8_t *recevbuf, size_t len);			
			inline void send(const uint8_t *sendbuf, size_t len) { sendReceive(sendbuf, NULL, len); }
			inline void receive(uint8_t *recvbuf, size_t len) { sendReceive(NULL, recvbuf, len); }
			
			void irq();
			virtual void irqcallback();
			
		private:
			int num;
			
			const uint8_t *sendbuf;
			size_t sendlen;
			uint8_t *recvbuf;
			size_t recvlen;
			
			Mutex mutex;
			Notifier notifier;
			
			IOPinConfig nss_pin, sck_pin, miso_pin, mosi_pin;
	};
}

#endif
