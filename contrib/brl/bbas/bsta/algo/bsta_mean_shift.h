// This is brl/bbas/bsta/algo/bsta_mean_shift.h
#ifndef bsta_mean_shift_h_
#define bsta_mean_shift_h_
//:
// \file
// \brief Classes to run mean shift algorithm on data distributions to find its modes
//        Implements mean shift with a flat kernel of fixed bandwidth_
//
// \author Ozge C. Ozcanli
// \date February 10, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//

#include <iostream>
#include <utility>
#include <bsta/bsta_parzen_sphere.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gaussian_sphere.h>
#include <bsta/bsta_gaussian_full.h>
#include <vnl/vnl_random.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bsta/algo/bsta_sample_set.h>

template <class T, unsigned n>
class bsta_mean_shift
{
 public:
  typedef typename bsta_parzen_sphere<T,n>::vector_type vector_;
  //typedef typename bsta_parzen_sphere<T,n>::covar_type var_t;

  //Constructor
  bsta_mean_shift() : max_iter_(1000) {}

  //: initializes seeds randomly and finds all the resulting modes
  //  \p epsilon : the difference required for updating to come to a halt
  //  \p percentage: the percentage of the sample set to initialize as seed
  bool find_modes(bsta_sample_set<T,n>& set, vnl_random & rng, float percentage = 10.0f, T epsilon = 10e-3);

  //: use all the samples to get its mode, no need for random seed picking
  bool find_modes(bsta_sample_set<T,n>& set, T epsilon = 10e-3);

  //: trim modes that are within epsilon distance to each other
  //  \p epsilon : the difference required for two modes to be merged
  bool trim_modes(bsta_sample_set<T,n>& set, T epsilon = 10e-3);

  //: merge the mode with samples less than cnt to one of the modes depending on its samples mean-shift paths
  bool merge_modes(bsta_sample_set<T,n>& set, int cnt, T epsilon);

  void clear() { modes_.clear(); }

  unsigned size() const { return modes_.size(); }
  std::vector<vector_ >& modes() { return modes_; }

  //: the default for maximum iterations to quit search starting from a seed is 1000
  void set_max_iter(unsigned iter) { max_iter_ = iter; }

 private:

  std::vector<vector_ > modes_;  // modes of the distribution

  unsigned max_iter_;
};

#endif // bsta_mean_shift_h_
