// This is oxl/osl/osl_canny_rothwell_params.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

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

