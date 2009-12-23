#ifndef FC_MATH_KALMAN_H
#define FC_MATH_KALMAN_H

#define KALMAN_DT .005

namespace FC {
	class Kalman {
		public:
			struct Config {
				float Q[3*3];
				float R[2*2];
			};

			union State {
				struct {
					float angle, vel, veloffset;
				};
				
				float vec[3];
			};

			union Measurement {
				struct {
					float accelangle, gyrovel;
				};
				
				float vec[2];
			};

			typedef float CovarianceMat[3*3];

			Kalman(const Config &config);
			
			void step(const Measurement &measurement);
			inline const State &getState() { return x; }
			inline const CovarianceMat &getCovariance() { return P; }
			
		private:
			const Config &config;
			State x;
			CovarianceMat P;
	};
}

#endif
