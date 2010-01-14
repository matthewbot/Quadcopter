#ifndef FC_MATH_PID_H
#define FC_MATH_PID_H

namespace FC {
	class PID {
		public:
			struct Config {
				float p, i, d;
				float i_max;
				float dt;
			} __attribute__((packed));
			
			PID(const Config &config);
			
			float update(float error);
			inline void clearInt() { int_error = 0; }
			
//		private:
			const Config &config;	
			float prev_error;
			float int_error;
	};
}

#endif
