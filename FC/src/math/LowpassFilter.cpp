#include "LowpassFilter.h"

using namespace FC;

LowpassFilter::LowpassFilter(float alpha, float startval) 
: alpha(alpha), prevval(startval) { }

float LowpassFilter::filter(float in) {
	float out = alpha*in + (1-alpha)*prevval;
	prevval = out;
	return out;
}

