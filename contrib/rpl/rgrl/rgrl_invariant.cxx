#include "rgrl_invariant.h"

#include <vcl_cassert.h>

vnl_vector<double> rgrl_invariant::zero_vec = vnl_vector<double>(0);

rgrl_mask_box
rgrl_invariant::
region() const
{
  assert( "region() not implemented in the base class" == 0 );
  return rgrl_mask_box(vnl_vector<double>(0), vnl_vector<double>(0)); // to suppress compiler warnings
}
