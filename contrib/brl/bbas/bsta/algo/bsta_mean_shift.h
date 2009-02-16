// This is brl/bbas/bsta/algo/bsta_mean_shift.h
#ifndef bsta_mean_shift_h_
#define bsta_mean_shift_h_
//:
// \file
// \brief Classes to run mean shift algorithm on data distributions to find its modes
// \author Ozge C. Ozcanli
// \date February 10, 2009
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim
//

#include <bsta/bsta_parzen_sphere.h>
#include <vnl/vnl_random.h>
#include <vcl_utility.h>

//: A class to hold samples and the window width parameter
template <class T, unsigned n>
class bsta_mean_shift_sample_set : public bsta_parzen_sphere<T,n>
{
 public:
  typedef typename bsta_parzen_sphere<T,n>::math_type T;
  typedef typename bsta_parzen_sphere<T,n>::vector_type vector_;
  enum { data_dimension = bsta_parzen_sphere<T,n>::dimension };

  // Constructor
  bsta_mean_shift_sample_set(T bandwidth = T(1)) : bsta_parzen_sphere() { set_bandwidth(bandwidth); }

  //: Compute the mean in a window around the given pt, the window size is the bandwidth
  //  If there are no points within bandwidth of the input pt, \return false
  bool mean(vector_ const& pt, vector_& out);
};

template <class T, unsigned n>
class bsta_mean_shift
{
 public:
  typedef typename bsta_parzen_sphere<T,n>::vector_type vector_;

  //Constructor
  bsta_mean_shift() : max_iter_(1000) {}

  //: initializes seeds randomly and finds all the resulting modes
  //  \p epsilon : the difference required for updating to come to a halt
  //  \p percentage: the percentage of the sample set to initialize as seed
  bool find_modes(bsta_mean_shift_sample_set<T,n>& set, vnl_random & rng, float percentage = 10.0f, T epsilon = 10e-3);

  //: trim modes that are within epsilon distance to each other
  //  \p epsilon : the difference required for two modes to be merged
  bool trim_modes(T epsilon = 10e-3);

  void clear() { modes_.clear(); }

  unsigned size() { return modes_.size(); }
  vcl_vector<vector_ >& modes() { return modes_; }

  vcl_vector<vcl_pair<bool, unsigned> >& assignments() { return assignment_; }

  //: the default for maximum iterations to quit search starting from a seed is 1000
  void set_max_iter(unsigned iter) { max_iter_ = iter; }

  //: recompute the modes, after establishing the assignment
  bool recompute_modes(bsta_mean_shift_sample_set<T,n>& set);

 private:

  vcl_vector<vector_ > modes_;  // modes of the distribution

  unsigned max_iter_;

  //: whether a mode is found for a given sample and the id of the mode in the modes_ vector
  vcl_vector<vcl_pair<bool, unsigned> > assignment_;
};


#endif // bsta_mean_shift_h_
