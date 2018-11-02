// This is brl/bbas/bsta/bsta_attributes.h
#ifndef bsta_attributes_h_
#define bsta_attributes_h_
//:
// \file
// \brief Attributes for distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 26, 2006
//
// Some algorithms require distributions to maintain additional
// attributes.  Attributes inherit from a distribution and
// add new functionality.
//
// do not remove the following text
// Approved for public release, distribution unlimited (DISTAR Case 14389)
//

#include <iostream>
#include "bsta_distribution.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vpdl/vpdt/vpdt_dist_traits.h>

//: Adds number of observations
template <class dist_>
class bsta_num_obs : public dist_
{
  typedef typename dist_::math_type T;

 public:

  typedef dist_ contained_type;

  //: Constructor
  bsta_num_obs<dist_>() : dist_(), num_observations(T(0)) {}

  //: Constructor - somewhat like a copy constructor
  bsta_num_obs<dist_>(const dist_& d, const T& n_obs = T(0))
    : dist_(d), num_observations(n_obs) {}

  //: The number of observations
  T num_observations;
};

template <class dist_>
inline std::ostream& operator<< (std::ostream& os,
                                bsta_num_obs<dist_> const& no)
{
  dist_ const& dist = static_cast<dist_ const&>(no);
  os << "n_obs:" << no.num_observations << '\n'
     << dist ;
  return os;
}

//: for compatibility with vpdl/vpdt
template <class dist>
struct vpdt_is_mixture<bsta_num_obs<dist> >
{
  static const bool value = vpdt_is_mixture<dist>::value;
};


//: maintains a vector sum and number of observations needed for the von mises distribution
template <class dist_>
class bsta_vsum_num_obs : public dist_
{
  typedef typename dist_::math_type T;
  typedef typename dist_::vector_type vect_t;

 public:

  typedef dist_ contained_type;

  //: Constructor
  bsta_vsum_num_obs<dist_>() : dist_(), num_observations(T(0)), vector_sum(vect_t(T(0))) {}

  //: Constructor - somewhat like a copy constructor
  bsta_vsum_num_obs<dist_>(const dist_& d, const vect_t & vsum = vect_t(T(0)), const T& n_obs = T(0))
  : dist_(d), num_observations(n_obs), vector_sum(vsum) {}

  //: The number of observations
  T num_observations;
  vect_t vector_sum;
};

template <class dist_>
inline std::ostream& operator<< (std::ostream& os,
                                bsta_vsum_num_obs<dist_> const& vno)
{
  dist_ const& dist = static_cast<dist_ const&>(vno);
  os << "n_obs:" << vno.num_observations << '\n'
     << "vector_sum:" << vno.vector_sum << '\n'
     << dist ;
  return os;
}

//: for compatibility with vpdl/vpdt
template <class dist>
struct vpdt_is_mixture<bsta_vsum_num_obs<dist> >
{
  static const bool value = vpdt_is_mixture<dist>::value;
};


#endif // bsta_attributes_h_
