#ifndef STMOS_PERIPHERALS_IOPIN_H
#define STMOS_PERIPHERALS_IOPIN_H

#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <stmos/util/IRQCallback.h>
#include <stmos/util/Notifier.h>
#include <stdint.h>

namespace stmos {
	class IOPinConfig : NonCopyable {
		public:
			enum Port : uint8_t {
				PORT_A,
				PORT_B,
				PORT_C,
				PORT_D
			};
	
			typedef uint8_t Pin;
			
			struct PortPin {
				Port port;
				Pin pin;
			} __attribute__((packed));
		
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
			
			inline IOPinConfig(Port port, Pin pin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false) {
				doSetup(port, pin, mode, pullup, alternatefunction);
			}
			inline IOPinConfig(const PortPin &portpin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false) {
				doSetup(portpin.port, portpin.pin, mode, pullup, alternatefunction);
			}
			
			void doSetup(Port port, Pin pin, Mode mode, PullUp pullup, bool alternatefunction);
	};

	class IOPin : public IOPinConfig {
		public:
			enum Edge {
				EDGE_RISING = 1,
				EDGE_FALLING = 2,
				EDGE_BOTH = 3
			};
		
			IOPin(Port port, Pin pin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false);
			IOPin(const PortPin &portpin, Mode mode, PullUp pullup=NONE, bool alternatefunction=false);
			
			bool read() const;
			inline operator bool() const { return read(); }
			inline bool operator!() const { return !read(); }
			
			void set(bool val=true);
			inline const IOPin &operator=(bool val) { set(val); return *this; }
			void pulse();
			
			int EXTIirq();
			void setupEXTI(Callback &call, Edge edge=EDGE_RISING, int pri=IRQ_PRIORITY_LOW);
			void disableEXTI();
						
		private:
			const Port port;
			const Pin pin;
	};
	
	class IOPinWait : public IOPin, Callback, IRQCallback {
		public:
			IOPinWait(Port port, Pin pin, PullUp pullup=NONE, Edge waitedge=EDGE_RISING);
			IOPinWait(const PortPin &portpin, PullUp pullup=NONE, Edge waitedge=EDGE_RISING);
	
			void wait();
			
			virtual void call(); // EXTI callback
			virtual void irqcallback(); // irq callback
	
		private:
			Notifier notifier;
	};
}

#endif
