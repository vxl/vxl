
#ifndef rgrl_initializer_h_
#define rgrl_initializer_h_

// \file
// \brief Base class for generating initial estimates.
// \author Chuck Stewart
// \date 12 Nov 2002
// 
// \verbatim
// Modifications:
// Jan 2004 Charlene: Added other non-virtual functions to hide view.
// 
// \endverbatim

#include "rgrl_transformation_sptr.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_view_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_initializer_sptr.h"
#include "rgrl_mask.h"

//: Interface for initial estimate generators.
class rgrl_initializer
  : public rgrl_object
{
public:
  virtual ~rgrl_initializer();

  //: Get next initial view.
  //
  // Return true iff an estimate is available.  The initial
  // transformation estimate must have a covariance matrix filled in
  // because it is used in matching
  //
  // This is the only virtual function implemented in the derived
  // classes.
  virtual bool next_initial( rgrl_view_sptr           & view,
                             rgrl_scale_sptr          & prior_scale) = 0;

  //: Get next set of initial iniformation without the notion of a view.
  //
  virtual bool next_initial( rgrl_mask_box            & from_image_roi,
                             rgrl_mask_box            & to_image_roi,
                             rgrl_mask_box            & current_region,
                             rgrl_mask_box            & global_region,
                             rgrl_estimator_sptr      & xform_estimator,
                             rgrl_transformation_sptr & xform_estimate,
                             unsigned                 & current_resolution,
                             rgrl_scale_sptr          & prior_scale);

  //: Get next set of initial information, assuming \a current_region is the region of interest for registration.
  //
  virtual bool next_initial( rgrl_mask_box            & current_region,
                             rgrl_estimator_sptr      & xform_estimator,
                             rgrl_transformation_sptr & xform_estimate,
                             unsigned                 & current_resolution,
                             rgrl_scale_sptr          & prior_scale);

  //: Get next set of initial information, assuming \a current_region is the region of interest for registration, and single-resolution.
  //
  virtual bool next_initial( rgrl_mask_box            & current_region,
                             rgrl_estimator_sptr      & xform_estimator,
                             rgrl_transformation_sptr & xform_estimate,
                             rgrl_scale_sptr          & prior_scale);

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer, rgrl_object );

};

#endif // rgrl_initializer_h_
