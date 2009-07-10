// This is brl/bbas/bsta/bsta_sampler.h
#ifndef bsta_sampler_h_
#define bsta_sampler_h_
//:
// \file
// \brief Various sampling techniques to aid bsta library
// \author Ozge C. Ozcanli
// \date December 09, 2008
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli - Dec 09, 2008 - Added an algorithm to sample from a "set of samples with given probabilities"
// \endverbatim

#include <vcl_vector.h>
#include <vcl_utility.h>
#include <bsta/bsta_joint_histogram.h>

template <class T>
class bsta_sampler
{
 public:
  //: put cnt samples into output vector wrt given probabilities
  //  The sum of probabilities should sum to 1 otherwise return false
  static bool sample(vcl_vector<T>& samples, vcl_vector<float>& p,
                     unsigned cnt, vcl_vector<T>& out);

  //: sample from a joint histogram treating it as a discrete prob distribution
  static bool sample(const bsta_joint_histogram<float>& jh, unsigned cnt, vcl_vector<vcl_pair<float, float> >& out);
};

#endif // bsta_sampler_h_
