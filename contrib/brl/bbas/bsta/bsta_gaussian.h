// This is brl/bbas/bsta/bsta_gaussian.h
#ifndef bsta_gaussian_h_
#define bsta_gaussian_h_
//:
// \file
// \brief A Gaussian distribution for use in a mixture model
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/25/06
//
// \verbatim
//  Modifications
//   (none yet)
// \endverbatim


#include "bsta_distribution.h"
#include <vnl/vnl_math.h>

//: forward declare vnl_vector_fixed
template<class T, unsigned n> class vnl_vector_fixed;


template<unsigned n>
struct two_pi_power
{
  static inline double value()
  { return 2.0*vnl_math::pi*two_pi_power<n-1>::value(); }
};

VCL_DEFINE_SPECIALIZATION
struct two_pi_power<0>
{
  static inline double value() { return 1.0; }
};


//: A Gaussian distribution
// used as a component of the mixture
template <class T, unsigned n>
class bsta_gaussian : public bsta_distribution<T,n>
{
  typedef typename bsta_distribution<T,n>::vector_type vector_;

 public:

  //: The mean of the distribution
  const vector_& mean() const { return mean_; }

  //: Set the mean of the distribution
  void set_mean(const vector_& mean) { mean_ = mean; }

 protected:
  bsta_gaussian() : mean_(T(0)) {}
  bsta_gaussian(const vector_& mean) : mean_(mean) {}

  //: The mean
  vector_ mean_;
};

#endif // bsta_gaussian_h_
