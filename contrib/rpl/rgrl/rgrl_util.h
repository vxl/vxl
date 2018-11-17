#ifndef rgrl_util_h_
#define rgrl_util_h_
//:
// \file
// \author Charlene Tsai / Chuck Stewart / Gehua Yang
// \date   Sep 2003

#include <iostream>
#include <iosfwd>
#include <vector>
#include <rgrl/rgrl_transformation_sptr.h>
#include <rgrl/rgrl_weighter_sptr.h>
#include <rgrl/rgrl_scale_sptr.h>
#include <rgrl/rgrl_match_set_sptr.h>
#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_mask_sptr.h>

#include <vnl/vnl_vector.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <rgrl/rgrl_fwd.h>

class rgrl_match_set;

//: Estimate the maximum overlap region/volume based on the curr_xform
rgrl_mask_box
rgrl_util_estimate_global_region(
          rgrl_mask_sptr      const&   from_image_roi,
          rgrl_mask_sptr      const&   to_image_roi,
          rgrl_mask_box       const&   current_region,
          rgrl_transformation const&   curr_xform,
          bool                         union_with_curr = false,
          double                       drastic_change_ratio = 1.75 );

//: Estimate the maximum overlap region/volume based on the inverse xfrom
//  The inverse xform transforms points on Fixed image coordinate to Moving image
rgrl_mask_box
rgrl_util_estimate_global_region_with_inverse_xform(
          rgrl_mask_sptr      const&   from_image_roi,
          rgrl_mask_sptr      const&   to_image_roi,
          rgrl_mask_box       const&   current_region,
          rgrl_transformation const&   inv_xform,
          bool                         union_with_curr = false,
          double                       drastic_change_ratio = 1.75 );

//: Estimate the change between the spreads of the from_feature_set and mapped_feature_set
double
rgrl_util_geometric_error_scaling( rgrl_match_set const& match_set );

double
rgrl_util_geometric_error_scaling( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets );

//: Estimate the change between the spreads of the from_feature_set and mapped_feature_set
bool
rgrl_util_geometric_scaling_factors( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                                     vnl_vector<double>& factors );

//: Estimate the change between the spreads of the from_feature_set and mapped_feature_set
bool
rgrl_util_geometric_scaling_factors( rgrl_match_set const& current_match_set,
                                     vnl_vector<double>& factors );

//:
//  Given an oriented, rectangular solid in any number of dimensions, represented by
//  the basis_directions and radii, find the pixel locations that are within the solid.
//  basis_dirs are unit vectors and normal to each other.
void
rgrl_util_extract_region_locations( vnl_vector< double >             const& center,
                                    std::vector< vnl_vector<double> > const& basis_dirs,
                                    vnl_vector< double >             const& basis_radii,
                                    std::vector< vnl_vector<int> >         & pixel_locations );

//: A simplified version of irls w/o scale re-estimation.
//  This takes single match set
//  The \a fast_remapping indicates whether to re-map the features (false),
//  or to re-map only the locations (true). This save the computations
//  on re-mapping scales, orientations.  However, the result may be
//  inaccurate due to out-of-date signature weights.
bool
rgrl_util_irls( const rgrl_match_set_sptr&              match_set,
                const rgrl_scale_sptr&                  scale,
                const rgrl_weighter_sptr&               weighter,
                rgrl_convergence_tester   const& conv_tester,
                const rgrl_estimator_sptr&              estimator,
                rgrl_transformation_sptr       & estimate,
                const bool                       fast_remapping = false,
                unsigned int                     debug_flag = 0);

//: A simplified version of irls w/o scale re-estimation.
//  This takes multiple match sets
//  The \a fast_remapping indicates whether to re-map the features (false),
//  or to re-map only the locations (true). This save the computations
//  on re-mapping scales, orientations.  However, the result may be
//  inaccurate due to out-of-date signature weights.
bool
rgrl_util_irls( rgrl_set_of<rgrl_match_set_sptr> const& match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& scales,
                std::vector<rgrl_weighter_sptr>   const& weighters,
                rgrl_convergence_tester          const& conv_tester,
                const rgrl_estimator_sptr&              estimator,
                rgrl_transformation_sptr&        estimate,
                const bool                       fast_remapping = false,
                unsigned int                     debug_flag = 0);

//: skip empty lines in input stream
void
rgrl_util_skip_empty_lines( std::istream& is );


#endif // rgrl_util_h_
