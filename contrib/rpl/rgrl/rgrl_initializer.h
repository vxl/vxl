#ifndef rgrl_initializer_h_
#define rgrl_initializer_h_
//:
// \file
// \brief Base class for generating initial estimates.
// \author Chuck Stewart
// \date 12 Nov 2002
//
// \verbatim
//  Modifications:
//   Jan 2004 Charlene: Added other non-virtual functions to hide view.
// \endverbatim

#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_view_sptr.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_initializer_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <rgrl/rgrl_mask_sptr.h>

//: Interface for initial estimate generators.
class rgrl_initializer
  : public rgrl_object
{
 public:
  rgrl_initializer() : use_prior_scale_from_match_(true)
  {   }
  
  virtual ~rgrl_initializer();

  //: Get next initial view.
  //
  // Return true iff an estimate is available.  The initial
  // transformation estimate must have a covariance matrix filled in
  // because it is used in matching
  //
  // This is the only pure virtual function implemented in the derived classes.
  virtual bool next_initial( rgrl_view_sptr           & view,
                             rgrl_scale_sptr          & prior_scale) = 0;

  //: Get next set of initial iniformation without the notion of a view.
  //
  bool next_initial( rgrl_mask_sptr           & from_image_roi,
                     rgrl_mask_sptr           & to_image_roi,
                     rgrl_mask_box            & current_region,
                     rgrl_mask_box            & global_region,
                     rgrl_estimator_sptr      & xform_estimator,
                     rgrl_transformation_sptr & xform_estimate,
                     unsigned                 & current_resolution,
                     rgrl_scale_sptr          & prior_scale);

  //: Get next set of initial information, assuming \a current_region is the region of interest for registration.
  //
  bool next_initial( rgrl_mask_box            & current_region,
                     rgrl_estimator_sptr      & xform_estimator,
                     rgrl_transformation_sptr & xform_estimate,
                     unsigned                 & current_resolution,
                     rgrl_scale_sptr          & prior_scale);

  //: Get next set of initial information, assuming \a current_region is the region of interest for registration, and single-resol.
  //
  bool next_initial( rgrl_mask_box            & current_region,
                     rgrl_estimator_sptr      & xform_estimator,
                     rgrl_transformation_sptr & xform_estimate,
                     rgrl_scale_sptr          & prior_scale);

  //: force the setting of prior scale
  //  scale > 0    set to this scale value
  //  scale == 0   scale pointer is NULL, which indicates to use unwgted scale estimate
  //  scale < 0    use whatever scale set by the match
  void set_prior_geometric_scale( double scale );
  
  // Defines type-related functions
  rgrl_type_macro( rgrl_initializer, rgrl_object );

 protected:
  //: Generate a prior scale depending on current settings
  rgrl_scale_sptr enforce_prior_scale( rgrl_scale_sptr match_scale );

 protected:
  rgrl_scale_sptr prior_scale_;
  bool            use_prior_scale_from_match_;
};

#endif // rgrl_initializer_h_
