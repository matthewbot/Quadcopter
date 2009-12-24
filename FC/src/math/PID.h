#ifndef FC_MATH_PID_H
#define FC_MATH_PID_H

namespace FC {
	class PID {
		public:
			struct Config {
				float p, i, d;
				float i_max;
				float dt;
			};
			
			PID(const Config &config);
			
			float update(float error);
			
		private:
			const Config &config;	
			float prev_error;
			float int_error;
	};
}

#endif
