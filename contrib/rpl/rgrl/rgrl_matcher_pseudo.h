#ifndef rgrl_matcher_pseudo_h_
#define rgrl_matcher_pseudo_h_
//:
// \file
// \author Chuck Stewart
// \date   Sept 2003

#include <rgrl/rgrl_matcher.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_transformation.h>
#include <rgrl/rgrl_scale.h>
#include <rgrl/rgrl_feature_sptr.h>
#include "rgrl_evaluator.h"
#include <rgrl/rgrl_mask_sptr.h>
#include <rgrl/rgrl_mask.h>
#include <vil/vil_image_view.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_double_2.h>
#include "rgrl_evaluator_sptr.h"
//#include <itkImage.h>

//: For each "from" pseudo / weak feature, use intensity based matching to construct the "to" features.
//
// This will map the "from" feature and its surrounding region using
// the current transformation, and search for the best
// intensity-region match to construct the "to" feature.


//  EvaluatorType, which perhaps should be a base class, takes two
//  voxel lists and calculates a similarity measure beween them.  This
//  says how well two subregions match. (F in notes)

template < class PixelType >
class rgrl_matcher_pseudo
  : public rgrl_matcher
{
 public:

//   typedef vnl_vector< PixelType > rgrl_pixel_type;

  class rgrl_mapped_pixel_type {
   public:
    vnl_double_2  location;
    double              intensity;
    double              weight;
  };

  typedef vcl_vector< rgrl_mapped_pixel_type > rgrl_mapped_pixel_vector_type;

  //: Initialize the matcher using 2d images. 
  //
  rgrl_matcher_pseudo( vil_image_view<PixelType> from_image,
                       vil_image_view<PixelType> to_image,
                       rgrl_evaluator_sptr evaluator,
                       rgrl_mask_sptr from_mask = 0,
                       rgrl_mask_sptr to_mask = 0 );

  //:  Match the features in the "from" image to the intensity in the "to" image. 
  //
  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&     from_features,
                   rgrl_feature_set const&     to_features,
                   rgrl_view const&            current_view,
                   rgrl_transformation const&  current_xform,
                   rgrl_scale const&           current_scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_pseudo, rgrl_matcher );

 private:
  //:  Map the intensities of the image region.
  void 
  map_region_intensities( rgrl_transformation      const& trans,
        rgrl_feature_sptr               feature_sptr,
        rgrl_mapped_pixel_vector_type & mapped_pixels ) const;

  //:  The actual work of mapping the region intensities.
  void
  map_region_intensities( vcl_vector< vnl_vector<int> > const& pixel_locations,
        rgrl_transformation           const& trans,
        rgrl_feature_sptr                    feature_sptr,
        rgrl_mapped_pixel_vector_type      & mapped_pixels) const;

  void
  match_mapped_region( rgrl_feature_sptr                    mapped_feature,
           rgrl_mapped_pixel_vector_type const& mapped_pixels,
           rgrl_scale                    const& current_scale,
           vcl_vector< rgrl_feature_sptr >    & matched_to_features,
           vcl_vector< double >               & match_weights ) const;

  double compute_response( vnl_double_2            const& mapped_location,
                 rgrl_mapped_pixel_vector_type const& mapped_pixels,
                 vnl_double_2            const& shift ) const;

 private:
//  typedef itkImage< PixelType, Dimension > ImageType;

  //  These are currently only for 2d images.  ITK templates across
  //  dimension.  VXL / rgrl does not.  Need to work with ITK images
  vil_image_view<PixelType> from_image_;
  vil_image_view<PixelType> to_image_;

  // When we try to search for the best match, we have to make sure
  // only the points inside the valid region are considered. The mask_
  // here seems to be duplicate with the mask_ in
  // rgrl_feature_set_location_masked.  But it seems to me that it
  // can't be avoid to put another mask_ here with the current design.
  // Also, this is also temporary for retina images. - Bess.
  rgrl_mask_sptr from_mask_;
  rgrl_mask_sptr to_mask_;
  rgrl_evaluator_sptr evaluator_;
};

#endif // rgrl_matcher_pseudo_h_
