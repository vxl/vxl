#include "rgrl_invariant.h"

#include <vcl_cassert.h>

rgrl_invariant::
rgrl_invariant()
  :zero_vec(vnl_vector<double>(0))
{}

const vnl_vector<double>&
rgrl_invariant::
cartesian_invariants() const
{
 return zero_vec;
}

const vnl_vector<double>&
rgrl_invariant::
angular_invariants() const
{
 return zero_vec;
}

rgrl_mask_box
rgrl_invariant::
region() const
{
  assert( "region() not implemented in the base class" == 0 );
  return rgrl_mask_box(vnl_vector<double>(0), vnl_vector<double>(0)); // to supress compiler warnings
}

bool
rgrl_invariant::
has_region() const
{
  return false;
}
