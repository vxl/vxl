#ifndef rgrl_initializer_prior_h_
#define rgrl_initializer_prior_h_
//:
// \file
// \brief Generate initial estimates based on a single prior transformation.
// \author Chuck Stewart
// \date 25 Nov 2002

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "rgrl_initializer.h"
#include "rgrl_estimator_sptr.h"
#include "rgrl_mask.h"
#include "rgrl_transformation_sptr.h"

//: Generate initial estimates based on a single prior transformation.
class rgrl_initializer_prior
  : public rgrl_initializer
{
 public:
  //: Initialize with a view
  rgrl_initializer_prior( const rgrl_view_sptr&            prior_view,
                          const rgrl_scale_sptr&           prior_scale = nullptr);

  //: Initialize with a set of information without packing everything into a view
  rgrl_initializer_prior(  rgrl_mask_sptr             const& from_image_roi,
                           rgrl_mask_sptr             const& to_image_roi,
                           rgrl_mask_box              const& init_from_region_roi,
                           const rgrl_estimator_sptr&        xform_estimator,
                           const rgrl_transformation_sptr&   xform_estimate,
                           unsigned                   resolution,
                           const rgrl_scale_sptr&            prior_scale = nullptr);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always
  rgrl_initializer_prior(  rgrl_mask_sptr             const& from_image_roi,
                           rgrl_mask_sptr             const& to_image_roi,
                           const rgrl_estimator_sptr&        xform_estimator,
                           const rgrl_transformation_sptr&   xform_estimate,
                           unsigned                   resolution = 0,
                           const rgrl_scale_sptr&            prior_scale = nullptr);

  //: Initialize with a set of information, assuming that registration applies to \a from_image_roi always;
  //  And \a from_image_roi and \a to_image_roi are the same
  rgrl_initializer_prior(  rgrl_mask_sptr             const& from_image_roi,
                           const rgrl_estimator_sptr&        xform_estimator,
                           const rgrl_transformation_sptr&   xform_estimate,
                           unsigned                   resolution = 0,
                           const rgrl_scale_sptr&            prior_scale = nullptr);

  //: Add more potential prior transformations
  void add_prior_xform( const rgrl_transformation_sptr&   xform_estimate );

  //: Get next initial estimate when first called, but return false thereafter.
  bool next_initial( rgrl_view_sptr           & view,
                     rgrl_scale_sptr          & prior_scale ) override;

  void reset_xform_index( ) { xform_index_ = 0; }

  //: return number of initializations
  //  It is always 1 for prior
  int size() const override
  { return 1; }

  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer_prior, rgrl_initializer );

 protected:
  rgrl_view_sptr           init_view_;
  rgrl_scale_sptr          prior_scale_;
  std::vector< rgrl_view_sptr > views_;
  unsigned int xform_index_;
};

#endif
