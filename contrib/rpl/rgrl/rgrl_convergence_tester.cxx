#include "rgrl_convergence_tester.h"

#include "rgrl_view.h"

rgrl_converge_status_sptr 
rgrl_convergence_tester::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view,
                rgrl_match_set_sptr                     current_match_set,
                rgrl_scale_sptr                         current_scale,
                bool                                    penalize_scaling )const
{
  rgrl_set_of<rgrl_match_set_sptr> match_sets;
  match_sets.push_back( current_match_set );
  rgrl_set_of<rgrl_scale_sptr> scales;
  scales.push_back( current_scale );
  return compute_status( prev_status, prev_view, current_view,
                         match_sets, scales, penalize_scaling );
}

rgrl_converge_status_sptr 
rgrl_convergence_tester::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_transformation_sptr                xform_estimate,
                rgrl_estimator_sptr                     xform_estimator,
                rgrl_set_of<rgrl_match_set_sptr> const& current_match_sets,
                rgrl_set_of<rgrl_scale_sptr>     const& current_scales,
                bool                                    penalize_scaling )const
{
  rgrl_mask_box   dummy_image_region(0);
  rgrl_view view( dummy_image_region, dummy_image_region, 
                  dummy_image_region, dummy_image_region,
                  xform_estimator, xform_estimate, 0);
  return compute_status( prev_status, view, view, 
                         current_match_sets, current_scales, penalize_scaling );
}
					                    
rgrl_converge_status_sptr 
rgrl_convergence_tester::
compute_status( rgrl_converge_status_sptr               prev_status,
                rgrl_transformation_sptr                xform_estimate,
                rgrl_estimator_sptr                     xform_estimator,
                rgrl_match_set_sptr                     current_match_set,
                rgrl_scale_sptr                         current_scale,
                bool                                    penalize_scaling )const
{
  rgrl_set_of<rgrl_match_set_sptr> match_sets;
  match_sets.push_back( current_match_set );
  rgrl_set_of<rgrl_scale_sptr> scales;
  scales.push_back( current_scale );
  return compute_status( prev_status, xform_estimate, xform_estimator,
                         match_sets, scales, penalize_scaling );
}	
