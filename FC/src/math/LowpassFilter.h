#ifndef FC_MATH_LOWPASSFILTER_H
#define FC_MATH_LOWPASSFILTER_H

namespace FC {
	class LowpassFilter {
		public:
			LowpassFilter(float alpha=0, float startval=0);
			float filter(float in);
			inline void setAlpha(float alpha) { this->alpha = alpha; }
			inline float getValue() { return prevval; }
			
		private:
			float alpha;
			float prevval;
	};
}

#endif
