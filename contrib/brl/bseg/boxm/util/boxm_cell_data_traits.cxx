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
bsta_num_obs<bsta_gauss_sf1> boxm_zero_val<short, bsta_num_obs<bsta_gauss_sf1> >()
{
  return {};
}

template <>
mix_gauss_sf1_type boxm_zero_val<short, mix_gauss_sf1_type>()
{
  return {};
}
