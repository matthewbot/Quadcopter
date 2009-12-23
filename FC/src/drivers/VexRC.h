#ifndef FC_DRIVERS_VEXRC_H
#define FC_DRIVERS_VEXRC_H

#include <stmos/peripherals/Timer.h>
#include <stmos/util/Callback.h>
#include <stmos/util/NonCopyable.h>
#include <stdint.h>

namespace FC {
	class VexRC : stmos::NonCopyable, stmos::Callback {
		public:
			typedef int8_t AnalogChannel;
			
			enum DigitalChannel : int8_t {
				NONE,
				UP,
				DOWN
			};
			
			union Channels {
				struct {
					AnalogChannel analogs[4];
					DigitalChannel left, right;
				};
				
				int8_t data[6];
			};
		
			VexRC(const stmos::Timer &tim, int chan);
			
			void calibrateCenters();
			
			bool getSynced() const;
			Channels getChannels() const;
			
		private:
			const stmos::Timer &tim;
			mutable stmos::InputCaptureTimerChannel capturechan;
			
			uint8_t centers[4];
			
			volatile bool synced;
			Channels channels;

			uint8_t currentchan;
			uint16_t prevtimerval;
			unsigned long prevtime;
			int8_t capturebuf[6];
			
			virtual void call();
			static DigitalChannel captureToDigital(uint8_t capture);
	};

}

#endif
