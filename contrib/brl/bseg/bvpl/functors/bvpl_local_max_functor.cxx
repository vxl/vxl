#include "bvpl_local_max_functor.h"
//:
// \file
// \brief Template specializations

template <>
void bvpl_local_max_functor<bvxm_opinion>::init()
{
  max_=bvxm_opinion(1.0,0.0);
}

template <>
void bvpl_local_max_functor<bsta_num_obs<bsta_gauss_sf1> >::init()
{
  max_= bsta_gauss_sf1(0.0f, 1.0f);
}

template <>
bool bvpl_local_max_functor<bsta_num_obs<bsta_gauss_sf1> >::greater_than(const bsta_num_obs<bsta_gauss_sf1>& g1, const bsta_num_obs<bsta_gauss_sf1>& g2)
{
  return (g1.mean() - g2.mean()) > 0.0f;
  //return std::abs(g1.mean()) - std::abs(g2.mean()) > std::numeric_limits<float>::epsilon();
}

template <>
bsta_num_obs<bsta_gauss_sf1> bvpl_local_max_functor<bsta_num_obs<bsta_gauss_sf1> >::min_response()
{
  return bsta_gauss_sf1(0.0f, 1.0f);
}

template <>
bvxm_opinion bvpl_local_max_functor<bvxm_opinion >::min_response()
{
  return bvxm_opinion(1.0f, 0.0f);
}

template <>
float bvpl_local_max_functor<bsta_num_obs<bsta_gauss_sf1> >::filter_response(unsigned id, unsigned target_id, bsta_num_obs<bsta_gauss_sf1> curr_val)
{
  if (id !=target_id)
    return 0.0f;

  return std::abs(curr_val.mean());
}
