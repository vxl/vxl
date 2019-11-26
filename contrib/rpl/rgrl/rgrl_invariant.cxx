#include "rgrl_invariant.h"

#include <cassert>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

vnl_vector<double> rgrl_invariant::zero_vec = vnl_vector<double>(0);

rgrl_mask_box
rgrl_invariant::
region() const
{
  assert( "region() not implemented in the base class" == nullptr );
  return rgrl_mask_box(vnl_vector<double>(0), vnl_vector<double>(0)); // to suppress compiler warnings
}
