// This is core/vpdl/vpdt/vpdt_log_probability.h
#ifndef vpdt_log_probability_h_
#define vpdt_log_probability_h_
//:
// \file
// \author Matthew Leotta
// \brief The basic functions for log of probability calculation
// \date March 13, 2009
//
// These functions provide default implementations for various log of
// probability calculation functions.  They are written in terms of distribution
// member functions
//
// \verbatim
//  Modifications
//   None
// \endverbatim

#include <limits>
#include <vpdl/vpdt/vpdt_dist_traits.h>
#include <vnl/vnl_math.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute the log of the unnormalized density
template <class dist>
inline typename vpdt_dist_traits<dist>::scalar_type
vpdt_log_density(const dist& d,
                 const typename vpdt_dist_traits<dist>::field_type& pt)
{
  typedef typename vpdt_dist_traits<dist>::scalar_type T;
  T density = d.density(pt);
  if (density <= T(0))
    return std::numeric_limits<T>::infinity();

  return static_cast<T>(std::log(density));
}


//: Compute the log of the normalized probability density
template <class dist>
inline typename vpdt_dist_traits<dist>::scalar_type
vpdt_log_prob_density(const dist& d,
                      const typename vpdt_dist_traits<dist>::field_type& pt)
{
  typedef typename vpdt_dist_traits<dist>::scalar_type T;
  T norm = d.norm_const();
  if (vnl_math::isinf(norm))
    return -std::numeric_limits<T>::infinity();

  return static_cast<T>(std::log(norm) + vpdt_log_density(d,pt));
}


//: Compute the gradient of the log of the unnormalized density
template <class dist>
inline typename vpdt_dist_traits<dist>::scalar_type
vpdt_gradient_log_density(const dist& d,
                          const typename vpdt_dist_traits<dist>::field_type& pt,
                          const typename vpdt_dist_traits<dist>::vector_type& g)
{
  typedef typename vpdt_dist_traits<dist>::scalar_type T;
  T density = d.gradient_density(pt,g);
  if (density <= T(0)) {
    vpdt_fill(g,T(0));
    return std::numeric_limits<T>::infinity();
  }

  g /= density;
  return static_cast<T>(std::log(density));
}


#endif // vpdt_log_probability_h_
