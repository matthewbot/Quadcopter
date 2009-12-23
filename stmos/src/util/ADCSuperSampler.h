#ifndef STMOS_UTIL_ADCSUPERSAMPLER_H
#define STMOS_UTIL_ADCSUPERSAMPLER_H

#include <stmos/peripherals/ADC.h>
#include <stmos/peripherals/DMA.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>

namespace stmos {
	class ADCSuperSampler : NonCopyable {
		public:
			ADCSuperSampler(ADC &adc, const ADC::Channel *chans, size_t chancount, int samples);
			~ADCSuperSampler();
			
			void superSample(ADC::Sample *samples);
		
		private:
			ADC &adc;
			DMA dma;
			
			size_t chancount;
			int samplecount;
			ADC::Sample *samplebuf;
	};

}


#endif
