#ifndef rgrl_util_h_
#define rgrl_util_h_
//:
// \file
// \author Charlene Tsai / Chuck Stewart / Gehua Yang
// \date   Sep 2003

#include "rgrl_mask.h"
#include "rgrl_transformation.h"
#include "rgrl_transformation_sptr.h"
#include "rgrl_weighter_sptr.h"
#include "rgrl_scale_sptr.h"
#include "rgrl_match_set_sptr.h"
#include "rgrl_set_of.h"
#include "rgrl_convergence_tester.h"

#include <vnl/vnl_vector.h>
#include <vcl_iosfwd.h>
#include <vcl_vector.h>

class rgrl_match_set;

//: Estimate the maximum overlap region/volumn based on the curr_xform
rgrl_mask_box
rgrl_util_estimate_global_region( rgrl_mask_box const&         from_image_roi,
                                  rgrl_mask_box const&         to_image_roi,
                                  rgrl_mask_box const&         current_region,
                                  rgrl_transformation const&   curr_xform,
                                  bool                         union_with_curr = false );

//: Estimate the change between the spreads of the from_feature_set and mapped_feature_set
double
rgrl_util_geometric_error_scaling( rgrl_match_set const& match_set );

double
rgrl_util_geometric_error_scaling( rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets );

//:
//  Given an oriented, rectangular solid in any number of dimensions, represented by
//  the basis_directions and radii, find the pixel locations that are within the solid.
//  basis_dirs are unit vectors and normal to each other.
void
rgrl_util_extract_region_locations( vnl_vector< double >             const& center,
                                    vcl_vector< vnl_vector<double> > const& basis_dirs,
                                    vnl_vector< double >             const& basis_radii,
                                    vcl_vector< vnl_vector<int> >         & pixel_locations );

//: A simplified version of irls w/o scale re-estimation.
//  This takes single match set
bool
rgrl_util_irls( rgrl_match_set_sptr              match_set,
                rgrl_scale_sptr                  scale,
                rgrl_weighter_sptr               weighter,
                rgrl_convergence_tester   const& conv_tester,
                rgrl_estimator_sptr              estimator,
                rgrl_transformation_sptr       & estimate,
                double                         & error,
                unsigned int                     debug_flag = 0);

//: A simplified version of irls w/o scale re-estimation.
//  This takes multiple match sets
bool
rgrl_util_irls( rgrl_set_of<rgrl_match_set_sptr> const& match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& scales,
                vcl_vector<rgrl_weighter_sptr>   const& weighters,
                rgrl_convergence_tester          const& conv_tester,
                rgrl_estimator_sptr              estimator,
                rgrl_transformation_sptr&        estimate,
                double&                          total_error,
                unsigned int                     debug_flag = 0);

//: skip empty lines in input stream
void
rgrl_util_skip_empty_lines( vcl_istream& is );


#endif // rgrl_util_h_
