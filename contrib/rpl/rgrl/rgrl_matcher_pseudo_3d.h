#ifndef rgrl_matcher_pseudo_3d_h_
#define rgrl_matcher_pseudo_3d_h_

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
#include <vil3d/vil3d_image_view.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_int_3.h>
#include "rgrl_evaluator_sptr.h"
#if 0
#include <itkImage.h>
#endif

//  EvaluatorType, which perhaps should be a base class, takes two
//  voxel lists and calculates a similarity measure beween them.  This
//  says how well two subregions match. (F in notes)

template < class PixelType >
class rgrl_matcher_pseudo_3d
  : public rgrl_matcher
{
 public:

  class rgrl_mapped_pixel_type
  {
   public:
    vnl_int_3  location;
    double     intensity;
    double     weight;
  };

  typedef vcl_vector< rgrl_mapped_pixel_type > rgrl_mapped_pixel_vector_type;

  //: Initialize the matcher using 3d images.
  //
  rgrl_matcher_pseudo_3d( vil3d_image_view<PixelType> const& from_image,
                          vil3d_image_view<PixelType> const& to_image,
                          vnl_vector< double > const& from_spacing_ratio,
                          vnl_vector< double > const& to_spacing_ratio,
                          rgrl_evaluator_sptr evaluator,
                          rgrl_mask_sptr mask = 0 );

  //:  Match the features in the "from" image to the intensity in the
  //  "to" image.
  //
  rgrl_match_set_sptr
  compute_matches( rgrl_feature_set const&     from_features,
                   rgrl_feature_set const&     to_features,
                   rgrl_view const&            current_view,
                   rgrl_transformation const&  current_xform,
                   rgrl_scale const&           current_scale ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_matcher_pseudo_3d, rgrl_matcher);

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

  double compute_response( vnl_double_3        const& mapped_location,
                           rgrl_mapped_pixel_vector_type const& mapped_pixels,
                           vnl_double_3                  const& shift ) const;

 private:
#if 0
  typedef itkImage< PixelType, Dimension > ImageType;
#endif

  //  These are currently only for 2d images.  ITK templates across
  //  dimension.  VXL / rgrl does not.  Need to work with ITK images
  vil3d_image_view<PixelType> from_image_;
  vil3d_image_view<PixelType> to_image_;

  // When we try to search for the best match, we have to make sure
  // only the points inside the valid region are considered. The mask_
  // here seems to be duplicate with the mask_ in
  // rgrl_feature_set_location_masked.  But it seems to me that it
  // can't be avoid to put another mask_ here with the current design.
  // Also, this is also temporary for retina images. - Bess.
  rgrl_mask_sptr mask_;
  rgrl_evaluator_sptr evaluator_;

  // The ratio of physical coordinates / pixel coordinates.
  // It represents the physical distance between pixels.
  vnl_double_3 from_spacing_ratio_;
  vnl_double_3 to_spacing_ratio_;
};

#endif // rgrl_matcher_pseudo_3d_h_
