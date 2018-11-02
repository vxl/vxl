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
//   Daniel E. Crispell - Feb. 22, 2010 - Added versions of functions that allow user to provide own vnl_random object
//   Brandon A. Mayer - Oct. 25, 2012 - Added static default random number generator. Consecutive calls to bsta_sampler
//       produced the same samples if time between calls was less than std::time resolution.
// \endverbatim

#include <vector>
#include <iostream>
#include <utility>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_random.h>
#include <bsta/bsta_joint_histogram.h>

template <class T>
class bsta_sampler
{
 public:
  //: put cnt samples into output vector wrt given probabilities
  //  The sum of probabilities should sum to 1 otherwise return false
  static bool sample(std::vector<T>& samples, std::vector<float>& p,
                     unsigned cnt, std::vector<T>& out);

  //: put cnt samples into output vector wrt given probabilities
  //  The sum of probabilities should sum to 1 otherwise return false.
  //  User provides seeded random generator
  static bool sample(std::vector<T>& samples, std::vector<float>& p,
                     unsigned cnt, std::vector<T>& out, vnl_random &rng);

  //: sample from a joint histogram treating it as a discrete prob distribution
  static bool sample(const bsta_joint_histogram<float>& jh, unsigned cnt, std::vector<std::pair<float, float> >& out);

  //: sample from a joint histogram treating it as a discrete prob distribution
  // User provides seeded random generator
  static bool sample(const bsta_joint_histogram<float>& jh, unsigned cnt, std::vector<std::pair<float, float> >& out, vnl_random &rng);

  //: sample in the decreasing order of likelihood (i.e. the most likely bin will be returned as the first sample)
  static bool sample_in_likelihood_order(const bsta_joint_histogram<float>& jh, unsigned cnt, std::vector<std::pair<float, float> >& out);

  //: sample in the decreasing order of likelihood (i.e. the most likely bin will be returned as the first sample)
  static bool sample_in_likelihood_order(const bsta_joint_histogram<float>& jh, unsigned cnt, std::vector<std::pair<unsigned, unsigned> >& out_indices);

  static vnl_random default_rand;
};

#endif // bsta_sampler_h_
