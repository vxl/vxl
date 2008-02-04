// This is brl/bbas/bsta/bsta_attributes.h
#ifndef bsta_attributes_h_
#define bsta_attributes_h_
//:
// \file
// \brief Attributes for distributions
// \author Matt Leotta (mleotta@lems.brown.edu)
// \date 1/26/06
//
// Some algorithms require distributions to maintain additions
// attributes.  Attributes inherit from a distribution and
// add new functionality.
//

#include "bsta_distribution.h"

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

#endif // bsta_attributes_h_
