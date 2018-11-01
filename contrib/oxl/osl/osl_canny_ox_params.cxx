// This is oxl/osl/osl_canny_ox_params.cxx
//:
// \file
// \author fsm

#include "osl_canny_ox_params.h"

osl_canny_ox_params::osl_canny_ox_params()
  : sigma(1.0f)
  , max_width(50)
  , gauss_tail(0.0001f)
  , low(2.0f)
  , high(12.0f)
  , edge_min(60)
  , min_length(10)
  , border_size(2)
  , border_value(0.0f)
  , scale(5.0f)
  , follow_strategy(2)
  , join_flag(true)
  , junction_option(0)
  , verbose(true)
{
}


osl_canny_ox_params::~osl_canny_ox_params() = default;
