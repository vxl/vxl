/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "vsl_canny_ox_params.h"

vsl_canny_ox_params::vsl_canny_ox_params() 
  : sigma(1.0)
  , max_width(50)
  , gauss_tail(0.0001)
  , low(2.0)
  , high(12.0)
  , edge_min(60)
  , min_length(10)
  , border_size(2)
  , border_value(0.0)
  , scale(5.0)
  , follow_strategy(2)
  , join_flag(true)
  , junction_option(0)
  , verbose(true)
{
}


vsl_canny_ox_params::~vsl_canny_ox_params() {
}
