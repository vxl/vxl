/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
#include "pair_float_int.h"

int pair_float_int_compare_ascend(pair_float_int const& p1, pair_float_int const& p2)
{
  if (p1.f < p2.f)
    return -1;
  
  if (p1.f > p2.f)
    return 1;

  return 0;
}

