#ifndef FC_CONTROL_TCCOMPASS_H
#define FC_CONTROL_TCCOMPASS_H

#include <FC/drivers/MicroMag.h>
#include <stmos/peripherals/ADC.h>
#include <stmos/util/NonCopyable.h>
#include <stmos/util/Callback.h>

namespace FC {
	class TCCompass : stmos::NonCopyable {
		public:
			TCCompass(MicroMag &mmag);
	
			float calcHeading(float roll, float pitch);
			
		private:
			MicroMag &mmag;
			float prevheading;
	};
};

#endif
