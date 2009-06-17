// This is brl/bbas/bsta/bsta_attributes.h
#ifndef bsta_attributes_h_
#define bsta_attributes_h_
//:
// \file
// \brief Attributes for distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date January 26, 2006
//
// Some algorithms require distributions to maintain additions
// attributes.  Attributes inherit from a distribution and
// add new functionality.
//

#include "bsta_distribution.h"
#include <vcl_iostream.h>
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

  // ======= inherited functions ===========

 // vnl_vector<T> mean() const { return (dynamic_cast<contained_type const*>(this))->mean(); }

  //T var() const { return (dynamic_cast<contained_type const*>(this))->var(); }
};

template <class dist_>
inline vcl_ostream& operator<< (vcl_ostream& os,
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

#endif // bsta_attributes_h_
