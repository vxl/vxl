#ifndef rgrl_initializer_prior_h_
#define rgrl_initializer_prior_h_
//:
// \file
// \brief Generate initial estimates based on a single prior transformation.
// \author Chuck Stewart
// \date 25 Nov 2002

#include "rgrl_initializer.h"
#include "rgrl_transformation.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_mask.h"

//: Generate initial estimates based on a single prior transformation.
class rgrl_initializer_prior
  : public rgrl_initializer
{
 public:
  //: Initialize with a view
  rgrl_initializer_prior( rgrl_view_sptr            prior_view,
                          rgrl_scale_sptr           prior_scale = 0);

  //: Initialize with a set of information without packing everything into a view
  rgrl_initializer_prior(  rgrl_mask_box              const& from_image_roi,
                           rgrl_mask_box              const& to_image_roi,
                           rgrl_mask_box              const& init_from_region_roi,
                           rgrl_estimator_sptr        xform_estimator,
                           rgrl_transformation_sptr   xform_estimate,
                           unsigned                   resolution,
                           rgrl_scale_sptr            prior_scale = 0);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always
  rgrl_initializer_prior(  rgrl_mask_box              const& from_image_roi,
                           rgrl_mask_box              const& to_image_roi,
                           rgrl_estimator_sptr        xform_estimator,
                           rgrl_transformation_sptr   xform_estimate,
                           unsigned                   resolution = 0,
                           rgrl_scale_sptr            prior_scale = 0);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always, and \a from_image_roi and \a to_image_roi are the same
  rgrl_initializer_prior(  rgrl_mask_box              const& from_image_roi,
                           rgrl_estimator_sptr        xform_estimator,
                           rgrl_transformation_sptr   xform_estimate,
                           unsigned                   resolution = 0,
                           rgrl_scale_sptr            prior_scale = 0);

  //: Get next initial estimate when first called, but return false thereafter.
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale );

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_prior, rgrl_initializer );

 protected:
  rgrl_view_sptr           init_view_;
  rgrl_scale_sptr          prior_scale_;
  bool                     called_before_;    //  has next_initial been called yet?
};

#endif
