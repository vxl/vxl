// This is core/vpdl/vpdl_distribution.hxx
#ifndef vpdl_distribution_hxx_
#define vpdl_distribution_hxx_
//:
// \file

#include <limits>
#include "vpdl_distribution.h"
#include <vpdl/vpdt/vpdt_access.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

//: Default implementation of numerical CDF inverse computation.
// This function is called by the virtual function inverse_cdf() by default
// in the univariate case.

template <class T>
T vpdl_compute_inverse_cdf(const vpdl_distribution<T,1>& /*dist*/, double /*p*/)
{
  // FIXME: implement CDF inverse computation here
  return T(0.0);
}


namespace{

//: Define helper classes to create partial specialization of member functions.
template <class T, unsigned int n>
class inverse_cdf_helper
{
 public:
  typedef typename vpdl_distribution<T,n>::vector vector;

  //: Do the actual inversion
  static inline vector invert(const vpdl_distribution<T,n>& /*dist*/, const T& /*p*/)
  {
    return vector(std::numeric_limits<T>::quiet_NaN());
  }
};

template <class T>
class inverse_cdf_helper<T,1>
{
 public:
  typedef typename vpdl_distribution<T,1>::vector vector;

  //: Do the actual inversion
  static inline vector invert(const vpdl_distribution<T,1>& dist, const T& p)
  {
    return vpdl_compute_inverse_cdf(dist,p);
  }
};

template <class T>
class inverse_cdf_helper<T,0>
{
 public:
  typedef typename vpdl_distribution<T,0>::vector vector;

  //: Do the actual inversion
  static inline vector invert(const vpdl_distribution<T,0>& dist, const T& /*p*/)
  {
    return vector(dist.dimension(), std::numeric_limits<T>::quiet_NaN());
  }
};

}


//: Compute the inverse of the cumulative_prob() function
// The value of x: P(x'<x) = P for x' drawn from the distribution.
// This is only valid for univariate distributions
// multivariate distributions will return -infinity
template <class T, unsigned int n>
typename vpdl_distribution<T,n>::vector
vpdl_distribution<T,n>::inverse_cdf(const T& p) const
{
  // use a static helper class to do partial specialization
  return inverse_cdf_helper<T,n>::invert(*this, p);
}


//: The probability of being in an axis-aligned box
// The box is defined by two points, the minimum and maximum.
// Implemented in terms of \c cumulative_prob() by default.
template <class T, unsigned int n>
T vpdl_distribution<T,n>::box_prob(const vector& min_pt,
                                   const vector& max_pt) const
{
  const unsigned int dim = this->dimension();

  // return zero for ill-defined box
  for (unsigned int j=0; j<dim; ++j){
    if (vpdt_index(max_pt,j)<=vpdt_index(min_pt,j))
      return T(0);
  }

  // this method is not tractable for large dimensions
  assert(sizeof(unsigned long)*8 > dim);
  // compute the number of corners of the box (2^dim)
  const unsigned long num_corners = 1 << dim;

  T prob = T(0);
  vector corner(min_pt);
  for (unsigned long i=0; i<num_corners; ++i)
  {
    // In odd dimensions, corners with an odd number of maximal axis
    // are added to the sum.  In even dimensions, corners with an even
    // number of maximal axis are added. The other corners are subtracted.
    bool plus = (dim%2 != 1);
    // create the corner position by selecting elements from max_pt and min_pt
    for (unsigned int j=0; j<dim; ++j){
      bool is_max = (i>>j) & 1;
      plus ^= is_max; // toggle plus if is_max
      vpdt_index(corner,j) = is_max?vpdt_index(max_pt,j):vpdt_index(min_pt,j);
    }
    if (plus)
      prob += cumulative_prob(corner);
    else
      prob -= cumulative_prob(corner);
  }

  return prob;
}


#undef VPDL_DISTRIBUTION_INSTANTIATE
#define VPDL_DISTRIBUTION_INSTANTIATE(T,n) \
template class vpdl_distribution<T,n >

// instantiate this function only for n == 1
#undef VPDL_INVERSE_CDF_INSTANTIATE
#define VPDL_INVERSE_CDF_INSTANTIATE(T) \
template T vpdl_compute_inverse_cdf(const vpdl_distribution<T,1>& dist, double p)

#endif // vpdl_distribution_hxx_
