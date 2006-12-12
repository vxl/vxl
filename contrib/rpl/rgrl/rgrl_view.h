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
//   Oct. 2006 - Gehua Yang - add a virtual function to scale the view by a scalar ratio
// \endverbatim


#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>

#include <rgrl/rgrl_view_sptr.h>
#include <rgrl/rgrl_feature_sptr.h>
#include <rgrl/rgrl_fwd.h>
#include <vcl_stlfwd.h>

//: Represents a "view" of the registration problem.
//
// A view captures the current region, the current transformation (and
// estimator), and the current resolution.
//
class rgrl_view
  : public rgrl_object
{
 public:
  
  typedef vcl_vector< rgrl_feature_sptr > feature_vector;
  
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

  //: return a self copy
  virtual
  rgrl_view_sptr
  self_copy() const;
  
  //:  Access "from" image region of interest
  rgrl_mask_sptr const& from_image_roi() const
  { return from_image_roi_; }

  //: set "from" image region of interest
  void set_from_image_roi(rgrl_mask_sptr const& roi)
  { from_image_roi_ = roi; }
    
  //:  Access "to" image region of interest
  rgrl_mask_sptr const& to_image_roi() const
  { return to_image_roi_; }

  //: set "to" image region of interest
  void set_to_image_roi(rgrl_mask_sptr const& roi)
  { to_image_roi_ = roi; }

  //:  Access current transformation region in "from" image coords
  rgrl_mask_box const& region() const
  { return current_region_; }
  
  //: set current region in "from" image coords
  void set_region( rgrl_mask_box const& box )
  { current_region_ = box; }

  //:  Access the approximated overlap region in "from" image coords
  rgrl_mask_box const& global_region() const
  { return global_region_; }

  //: set the approximated overlap region in "from" image coords
  void set_global_region( rgrl_mask_box const& global_region )
  { global_region_ = global_region; }
  
  //:  Access current transform estimator
  rgrl_estimator_sptr xform_estimator() const
  { return xform_estimator_; }
  
  //: set current transform estimator
  void set_xform_estimator( rgrl_estimator_sptr const& est)
  { xform_estimator_ = est; }

  //:  Access current transform estimate
  rgrl_transformation_sptr xform_estimate() const
  { return xform_estimate_; }
  
  //: set current transform estimate
  void set_xform_estimate( rgrl_transformation_sptr const& xform )
  { xform_estimate_ = xform; }

  //:  Access current REVERSE transform estimate
  rgrl_transformation_sptr inverse_xform_estimate() const
  { return inverse_estimate_; }
  
  //:  Set REVERSE transform estimate
  void set_inverse_xform_estimate(rgrl_transformation_sptr const& inverse)
  { inverse_estimate_ = inverse; }

  //:  Access the current resolution at which registration is working
  unsigned resolution() const
  { return current_resolution_; }
  
  //:  Set current resolution
  void set_resolution( unsigned res )
  { current_resolution_ = res; }

  //:  Return true if the current resolution is the finest level
  bool is_at_finest_resolution() const;

  //: get number of updates on global region
  int num_updates_global_region() const
  { return num_updates_global_region_; }
  
  //: set number of updates on global region
  //  shall be done inside view generator
  void set_num_updates_global_region(int num)
  { num_updates_global_region_ = num; }

  //: extract features within current region
  //  feature vector will be cleared before the operation
  virtual
  bool
  features_in_region( feature_vector& features, rgrl_feature_set const& fea_set ) const;
  
  //:  Return true if the current_region_ reached the global_region_
  virtual 
  bool current_region_converged() const;

  //:  Return true if all components except the xform_estimate_ are the same
  //
  virtual
  bool regions_converged_to(const rgrl_view& other) const;

  //:  Return true if both xform_estimator_ and xform_estimate_ are not null
  virtual
  bool is_valid() const;

  //: scale a view by a ratio
  virtual
  rgrl_view_sptr scale_by( unsigned new_resol, double scaling ) const;

  virtual
  bool
  inside_current_region( vnl_vector<double> const& loc );
  
#if 0
  //: True iff all the components are equal.
  bool operator==( const rgrl_view& other ) const;

  //: True iff some of the components are not equal.
  bool operator!=( const rgrl_view& other ) const;
#endif

  // Defines type-related functions
  rgrl_type_macro( rgrl_view, rgrl_object );

 protected:
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
  int                        num_updates_global_region_;
};


#endif // rgrl_view_h_
