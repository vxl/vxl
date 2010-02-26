#include "boxm_cell_data_traits.h"
//:
// \file


//: Template specialization of zero_val for floating point case
template <>
float boxm_zero_val<short, float>()
{
  return 0.0f;
}


//: Template specialization of zero_val for unimodal gaussian
template <>
bsta_num_obs<bsta_gauss_f1> boxm_zero_val<short, bsta_num_obs<bsta_gauss_f1> >()
{
  return bsta_num_obs<bsta_gauss_f1>();
}
