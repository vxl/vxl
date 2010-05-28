// This is core/vpdl/vpdt/vpdt_num_obs.h
#ifndef vpdt_num_obs_h_
#define vpdt_num_obs_h_
//:
// \file
// \author Matthew Leotta
// \brief Attach a "number of observations" variable to any distribution
// \date April 7, 2009
//
// \verbatim
//  Modifications
//   None
// \endverbatim


#include <vpdl/vpdt/vpdt_field_traits.h>

//: Attach a "number of observations" variable to any distribution.
//  The number of observations is non-integral because there may be
//  partial or weighted observations.
template <class dist_t>
class vpdt_num_obs : public dist_t
{
 public:
  //: the data type to represent a point in the field
  typedef typename dist_t::field_type field_type;
  //: define the parent type
  typedef dist_t parent_type;
  //: define the scalar type (normally specified by template parameter T)
  typedef typename vpdt_field_traits<field_type>::scalar_type T;

  //: Constructor
  vpdt_num_obs<dist_t>() : dist_t(), num_observations(T(0)) {}

  //: Constructor - somewhat like a copy constructor
  vpdt_num_obs<dist_t>(const dist_t& d, const T& n_obs = T(0))
  : dist_t(d), num_observations(n_obs) {}

  //: The number of observations
  T num_observations;
};


#endif // vpdt_num_obs_h_
