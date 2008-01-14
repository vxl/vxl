// This is brcv/seg/bsta/bsta_attributes.h
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
template <class _dist>
class bsta_num_obs : public _dist
{
  typedef typename _dist::math_type T;
  public:
    //: Constructor
    bsta_num_obs<_dist>() : _dist(), num_observations(T(0)) {}

    //: Constructor - somewhat like a copy constructor
    bsta_num_obs<_dist>(const _dist& d, const T& n_obs = T(0))
      : _dist(d), num_observations(n_obs) {}

    //: The number of observations
    T num_observations;
};



#endif // bsta_attributes_h_
