/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_canny_rothwell_params.h"

vsl_canny_rothwell_params::vsl_canny_rothwell_params() 
  : sigma(1.0)
  , low(2.0)
  , high(12.0)
  , range(2.0)
  , verbose(true)
{
}

vsl_canny_rothwell_params::~vsl_canny_rothwell_params() { }

