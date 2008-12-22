// This is brl/bbas/bsta/algo/bsta_sampler.txx
#ifndef bsta_sampler_txx_
#define bsta_sampler_txx_
//:
// \file

#include "bsta_sampler.h"
#include <vcl_cstdlib.h>

//: For sorting pairs by their first element
class first_less {
public:
bool
operator()( const vcl_pair<float,unsigned>& left,
            const vcl_pair<float,unsigned>& right )
{
  return left.first < right.first;
} 
};

//: put cnt samples into output vector wrt given probabilities
//  the sum of probabilities should sum to 1 otherwise return false
template <class T>
bool bsta_sampler<T>::sample(vcl_vector<T>& samples, vcl_vector<float>& p, unsigned cnt, vcl_vector<T>& out) {
  if (p.size() != samples.size())
    return false;
  if (!p.size())
    return false;

  vcl_set<vcl_pair<float, unsigned>, first_less > accum_p; 
  accum_p.insert(vcl_pair<float, unsigned>(p[0], 0));  // put the id of the sample
  
  for (unsigned i = 1; i < p.size(); i++) {
    float accum = (*(accum_p.rbegin())).first + p[i];
    accum_p.insert(vcl_pair<float, unsigned>(accum, i));
  }
  float last_val = (*(accum_p.rbegin())).first;
  float dif = last_val-1.0f;
  float abs_dif = dif < 0 ? -dif : dif;
  if (abs_dif > 0.1f)
    return false;

  vnl_random rand;
  for (unsigned i = 0; i < cnt; i++) {
    float r = (float)(rand.drand32());
    vcl_pair<float, unsigned> search_key(r, 0);
    unsigned r_id = (*(accum_p.upper_bound(search_key))).second;
    out.push_back(samples[r_id]);
  }
  
  return true;
}

#undef BSTA_SAMPLER_INSTANTIATE
#define BSTA_SAMPLER_INSTANTIATE(T) \
template class bsta_sampler<T >

#endif // bsta_sampler_txx_
