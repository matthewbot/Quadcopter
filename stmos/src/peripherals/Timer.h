#ifndef STMOS_PERIPHERALS_TIMER_H
#define STMOS_PERIPHERALS_TIMER_H

#include <stmos/peripherals/IOPin.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>
#include <stdint.h>

namespace stmos {
	class Timer : NonCopyable {
		public:
			enum Direction {
				DIR_UP,
				DIR_DOWN
			};
			
			Timer(int num, int pri=IRQ_PRIORITY_LOW);
			
			inline int getNum() const { return num; }
			
			void setTickTime(unsigned int cycles);
			void setDirection(Direction dir);
			void setOverflow(uint16_t overflow);
			
			uint16_t getOverflow() const;
			
			void start();
			void stop();
			
			uint16_t read();
			
		protected:
			int num;
	};
	
	class TimerChannel : NonCopyable {
		public:
			TimerChannel(int timernum, int num, IOPin::Mode mode);
			
			void setCCR(uint16_t ccr);
			uint16_t getCCR() const;
			void setCallback(Callback &callback);
			
			void setEnabled(bool enabled);
			inline void enable() { setEnabled(true); }
			inline void disable() { setEnabled(false); }
			
			void setIRQEnabled(bool enabled);
			inline void enableIRQ() { setIRQEnabled(true); }
			inline void disableIRQ() { setIRQEnabled(false); }
			
		protected:
			void setPolarity(bool pol);
		
			int timernum, num;
			IOPinConfig pin;
	};
	
	class InputCaptureTimerChannel : public TimerChannel {
		public:
			enum Filter {
				FILTER_NONE,
				FILTER_CK_2,
				FILTER_CK_4,
				FILTER_CK_8, 
				FILTER_DTS2_6,
				FILTER_DTS2_8,
				FILTER_DTS4_6,
				FILTER_DTS4_8,
				FILTER_DTS8_6,
				FILTER_DTS8_8,
				FILTER_DTS16_5,
				FILTER_DTS16_6,
				FILTER_DTS16_8,
				FILTER_DTS32_5,
				FILTER_DTS32_6,
				FILTER_DTS32_8			
			};
			
			enum Edge {
				EDGE_RISING,
				EDGE_FALLING
			};
		
			InputCaptureTimerChannel(int numternum, int num);
			
			void setFilter(Filter filter);
			inline void setEdge(Edge edge) { setPolarity((bool)edge); }
	};
	
	class OutputCompareTimerChannel : public TimerChannel {
		public:
			enum Mode {
				MODE_FROZEN,
				MODE_ACTIVE_MATCH,
				MODE_INACTIVE_MATCH,
				MODE_TOGGLE_MATCH,
				MODE_FORCE_INACTIVE,
				MODE_FORCE_ACTIVE,
				MODE_PWM_1,
				MODE_PWM_2
			};
		
			OutputCompareTimerChannel(int timernum, int num);
			
			void setMode(Mode mode);
			inline void setInverted(bool inverted) { setPolarity(inverted); }
	};
}

#endif
