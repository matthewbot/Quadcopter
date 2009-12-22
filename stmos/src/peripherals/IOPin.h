#ifndef STMOS_PERIPHERALS_IOPIN_H
#define STMOS_PERIPHERALS_IOPIN_H

#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <stdint.h>

namespace stmos {
	class IOPinConfig : NonCopyable {
		public:
			enum Port {
				PORT_A,
				PORT_B,
				PORT_C,
				PORT_D
			};
	
			typedef uint8_t Pin;
		
			enum Mode {
				INPUT,
				INPUT_ANALOG,
				OUTPUT,
				OUTPUT_OPENDRAIN
			};
		
			enum PullUp {
				NONE,
				PULLUP,
				PULLDOWN
			};
			
			IOPinConfig(Port port, Pin pin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false);
	};

	class IOPin : public IOPinConfig {
		public:
			enum Edge {
				EDGE_RISING = 1,
				EDGE_FALLING = 2,
				EDGE_BOTH = 3
			};
		
			IOPin(Port port, Pin pin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false);
			
			bool read() const;
			
			void set(bool val=true) const;
			inline const IOPin &operator=(bool val) const { set(val); return *this; }
			
			void setupEXTI(Callback &call, Edge edge, int pri=IRQ_PRIORITY_LOW);
			void disableEXTI();
						
		private:
			const Port port;
			const Pin pin;
	};
}

#endif
