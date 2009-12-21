#ifndef STMOS_PERIPHERALS_ADC_H
#define STMOS_PERIPHERALS_ADC_H

#include <stmos/util/NonCopyable.h>
#include <cstddef>
#include <stdint.h>

namespace stmos {
	class ADC : NonCopyable {
		public:
			typedef uint8_t Channel;
			typedef uint16_t Sample;
		
			enum SampleTime {
				SAMPLETIME_1,
				SAMPLETIME_7,
				SAMPLETIME_13,
				SAMPLETIME_28,	
				SAMPLETIME_41,
				SAMPLETIME_55,
				SAMPLETIME_71,
				SAMPLETIME_239
			};
		
			ADC(int num);
			
			// config
			void setSampleTime(Channel chan, SampleTime time);
			
			// single sample
			void setSampleChannel(Channel chan);
			Sample sample();
			inline Sample sampleChannel(Channel chan) { setSampleChannel(chan); return sample(); }		
			
			// multi sample (max is 4)
			void setSampleChannels(const Channel *chan, size_t chancount);
			void sampleMultiple(Sample *samples, size_t samplecount);

			// scan (max is 16)
			void setScanChannels(const Channel *chans, size_t chancount);
			void startScan();
			void stopScan();
			volatile void *getScanDMAAddress() const;
			int getScanDMAChannel() const;
			
		protected:
			int num;
	};
}

#endif
