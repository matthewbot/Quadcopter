#include "QuadcopterBase.h"
#include <FC/util/configs.h>

using namespace FC;
using namespace stmos;

QuadcopterBase::QuadcopterBase() 
: out(1, 115200),
  adc(1),
  adc2(2),
  
  analog(adc, configs::chans, configs::analog),
  
  mag(2, (IOPin::PortPin) { IOPin::PORT_C, 5 }, (IOPin::PortPin) { IOPin::PORT_B, 10 }),
  compass(mag),
  
  imu(analog, compass, configs::imu),
  
  motors(esctim),
  controlconfig(configs::control),
  motcontrol(controlconfig, imu, motors, esctim),
  
  vexrc(ppmtim, 4),
  
  batmon(adc2, 13) { }
  
