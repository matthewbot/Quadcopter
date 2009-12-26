#ifndef FC_DRIVERS_EEPROM_H
#define FC_DRIVERS_EEPROM_H

#include <stmos/peripherals/I2C.h>
#include <stmos/util/NonCopyable.h>
#include <cstddef>
#include <stdint.h>

namespace FC {
	class EEPROM : stmos::NonCopyable {
		public:
			EEPROM(stmos::I2C &i2c, stmos::I2C::Address i2caddr);
			
			typedef uint16_t Address;
			static const int pagelength = 128;
			static inline Address pageAddress(int page) { return (Address)(page * pagelength); }
			static inline Address byteAddress(int page, int byte) { return pageAddress(page) + byte; }
			
			void read(Address addr, uint8_t *data, size_t len);
			void write(Address addr, const uint8_t *data, size_t len);
			
		private:
			stmos::I2C &i2c;
			stmos::I2C::Address i2caddr;
			
			unsigned long lastwritetime;
			
			void setAddress(Address addr);
	};
}

#endif
