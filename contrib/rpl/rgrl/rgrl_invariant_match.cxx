#include "rgrl_invariant_match.h"

#include <vcl_cassert.h>

rgrl_invariant_match::
rgrl_invariant_match( rgrl_invariant_sptr from,
                      rgrl_invariant_sptr to)
  : from_(from),
    to_(to),
    is_estimate_set_(false)
{ 
  assert(from->cartesian_invariants().size() == to->cartesian_invariants().size() && 
         from->angular_invariants().size() == to->angular_invariants().size());
  distance_ = 0;

  // distance of cartesian components
  const vnl_vector<double>& from_inv_cart = from->cartesian_invariants();
  const vnl_vector<double>& to_inv_cart = to->cartesian_invariants();
  double dist_cartesian = vnl_vector_ssd(from_inv_cart, to_inv_cart);
  
  // distance of angular components
  const vnl_vector<double>& from_inv_ang = from->angular_invariants();
  const vnl_vector<double>& to_inv_ang = to->angular_invariants();
  double dist_angular = vnl_vector_ssd(from_inv_ang, to_inv_ang);

  distance_ = vcl_sqrt(dist_cartesian + dist_angular);
}

rgrl_transformation_sptr
rgrl_invariant_match::
transform() const
{
  assert( is_estimate_set_ );
  return xform_;
}

rgrl_scale_sptr
rgrl_invariant_match::
scale()const
{
  assert( is_estimate_set_ );
  return scale_;
}

rgrl_mask_box 
rgrl_invariant_match::
initial_region() const
{
  return from_->region();
}

bool 
rgrl_invariant_match::
has_initial_region() const
{
  return from_->has_region();
}

bool
rgrl_invariant_match::
estimate() 
{
  if ( !is_estimate_set_ ) 
    is_estimate_set_ = to_->estimate( from_, xform_, scale_ );

  return is_estimate_set_;
}

//----------------------------------------------------------------------- 
// Non-Member Functions

bool operator < (const rgrl_invariant_match& left,
                 const rgrl_invariant_match& right)
{
  return left.distance() < right.distance();
}

bool operator > (const rgrl_invariant_match& left,
                 const rgrl_invariant_match& right)
{
  return left.distance() > right.distance();
}

bool dist_less (const vbl_smart_ptr<rgrl_invariant_match>& left,
                const vbl_smart_ptr<rgrl_invariant_match>& right)
{
  return left->distance() < right->distance();
}

bool dist_greater (const vbl_smart_ptr<rgrl_invariant_match>& left,
                   const vbl_smart_ptr<rgrl_invariant_match>& right)
{
  return left->distance() > right->distance();
}

