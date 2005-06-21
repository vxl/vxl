#ifndef rgrl_view_h_
#define rgrl_view_h_
//:
// \file
// \brief  Represent a "view" of the registration problem.
// \author Chuck Stewart
// \date 12 Nov 2002
// \verbatim
// Modifications
//   June.2005 - Gehua Yang - change image ROI from rgrl_mask_box to rgrl_mask_sptr
// \endverbatim


#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>

#include <rgrl/rgrl_view_sptr.h>

//: Represents a "view" of the registration problem.
//
// A view captures the current region, the current transformation (and
// estimator), and the current resolution.
//
class rgrl_view
  : public rgrl_object
{
 public:
  
  //: ctor
  rgrl_view();
  
  //:  Build a view for registration estimation problem.
  rgrl_view( rgrl_mask_sptr      const& from_image_roi,
             rgrl_mask_sptr      const& to_image_roi,
             rgrl_mask_box       const& region,
             rgrl_mask_box       const& global_region,
             rgrl_estimator_sptr        xform_estimator,
             rgrl_transformation_sptr   xform_estimate,
             unsigned                   resolution = 0,
             rgrl_transformation_sptr   inverse_estimate = 0 );

  // default copy and assignment are good.

  //:  Access "from" image region of interest
  rgrl_mask_sptr const& from_image_roi() const;

  //:  Access "to" image region of interest
  rgrl_mask_sptr const& to_image_roi() const;

  //:  Access current transformation region in "from" image coords
  rgrl_mask_box const& region() const;

  //:  Access the approximated overlap region in "from" image coords
  rgrl_mask_box const& global_region() const;

  //:  Access current transform estimator
  rgrl_estimator_sptr xform_estimator() const;

  //:  Access current transform estimate
  rgrl_transformation_sptr xform_estimate() const;

  //:  Access current REVERSE transform estimate
  rgrl_transformation_sptr inverse_xform_estimate() const;
  
  //:  Set REVERSE transform estimate
  void set_inverse_xform_estimate(rgrl_transformation_sptr const& inverse)
  { inverse_estimate_ = inverse; }

  //:  Access the current resolution at which registration is working
  unsigned resolution() const;

  //:  Return true if the current resolution is the finest level
  bool is_at_finest_resolution() const;

  //:  Return true if the current_region_ reached the global_region_
  bool current_region_converged() const;

  //:  Return true if all components except the xform_estimate_ are the same
  //
  bool regions_converged_to(const rgrl_view& other) const;

  //:  Return true if both xform_estimator_ and xform_estimate_ are not null
  bool is_valid() const;

#if 0
  //: True iff all the components are equal.
  bool operator==( const rgrl_view& other ) const;

  //: True iff some of the components are not equal.
  bool operator!=( const rgrl_view& other ) const;
#endif

  // Defines type-related functions
  rgrl_type_macro( rgrl_view, rgrl_object );

 private:
  rgrl_mask_sptr                  from_image_roi_;
  rgrl_mask_sptr                  to_image_roi_;

  rgrl_mask_box                   current_region_;
  rgrl_mask_box                   global_region_; //The estimated overlap region
                                             //of from_image_roi_ with
                                             //to_image_roi_

  rgrl_estimator_sptr        xform_estimator_;
  rgrl_transformation_sptr   xform_estimate_;
  rgrl_transformation_sptr   inverse_estimate_;
  
  unsigned                   current_resolution_;
};


#endif // rgrl_view_h_
