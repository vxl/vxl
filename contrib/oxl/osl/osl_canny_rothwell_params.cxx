/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "osl_canny_rothwell_params.h"

osl_canny_rothwell_params::osl_canny_rothwell_params() 
  : sigma(1.0)
  , low(2.0)
  , high(12.0)
  , range(2.0)
  , verbose(true)
{
}

osl_canny_rothwell_params::~osl_canny_rothwell_params() { }

