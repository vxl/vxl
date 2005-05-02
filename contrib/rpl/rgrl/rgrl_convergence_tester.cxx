#include "rgrl_convergence_tester.h"
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_converge_status.h>
#include <vcl_cassert.h>

rgrl_convergence_tester::
~rgrl_convergence_tester()
{
}

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


rgrl_converge_status_sptr
rgrl_convergence_tester::
verify( rgrl_transformation_sptr    const& xform_estimate,
        rgrl_match_set_sptr         const& current_match_set,
        rgrl_scale_sptr             const& current_scale )const
{
  rgrl_set_of<rgrl_match_set_sptr> match_sets;
  match_sets.push_back( current_match_set );
  rgrl_set_of<rgrl_scale_sptr> scales;
  scales.push_back( current_scale );
  return this->verify( xform_estimate, match_sets, scales );
}

rgrl_converge_status_sptr
rgrl_convergence_tester::
verify( rgrl_transformation_sptr         const& /*xform_estimate*/,
        rgrl_set_of<rgrl_match_set_sptr> const& /*current_match_sets*/,
        rgrl_set_of<rgrl_scale_sptr>     const& /*current_scales*/ )const
{
  assert( !"should never call rgrl_convergence_tester::verify()" );
  return 0;
}

rgrl_converge_status_sptr
rgrl_convergence_tester::
initialize_status( rgrl_transformation_sptr                xform_estimate,
                   rgrl_estimator_sptr                     xform_estimator,
                   rgrl_scale_sptr                  const& prior_scale,
                   bool                                    penalize_scaling )const
{
  rgrl_mask_box   dummy_image_region(0);
  rgrl_view view( dummy_image_region, dummy_image_region,
                  dummy_image_region, dummy_image_region,
                  xform_estimator, xform_estimate, 0);
  return this->init_status( view, prior_scale, penalize_scaling );
}

rgrl_converge_status_sptr
rgrl_convergence_tester::
initialize_status( rgrl_view       const& init_view,
                   rgrl_scale_sptr const& prior_scale,
                   bool                   penalize_scaling ) const
{
  // call the real virtual function
  return this->init_status( init_view, prior_scale, penalize_scaling );
}

rgrl_converge_status_sptr
rgrl_convergence_tester::
init_status( rgrl_view       const& /*init_view*/,
             rgrl_scale_sptr const& /*prior_scale*/,
             bool                   /*penalize_scaling*/ ) const
{
  // nothing to initialize
  return 0;
}

rgrl_converge_status_sptr
rgrl_convergence_tester::
compute_status_helper( double new_error,
                       bool good_enough,
                rgrl_converge_status_sptr               prev_status,
                rgrl_view                        const& prev_view,
                rgrl_view                        const& current_view ) const
{
  // Step3: First check if the regions in the view have converged. If yes,
  //        check the convergence of the transform estimate by comparing to
  //        the previous status. The oscillation count is incremented if the
  //        error_diff changes the sign.
  //
  bool converged;
  bool stagnated = false;
  unsigned int oscillation_count = 0;
  double error_diff = 0.0;
  if (new_error == 0.0 )
    converged = true;
  else if ( prev_status &&  current_view.regions_converged_to(prev_view) ) {
    double old_error = prev_status->error();
    error_diff = new_error-old_error;
    double diff = (new_error-old_error) / new_error ;
    converged = vcl_abs( diff ) < rel_tol_thres_;

    if ( !converged ) {
      // look for oscillation
      // There are two kinds of oscillation:
      // 1. the error increases and decreases in turn
      // 2. Once it is good enough, the error increases slightly
      //

      // first situation
      if ( !good_enough ) {
        if ( error_diff * prev_status->error_diff() < 0.0 ) {
          oscillation_count = prev_status->oscillation_count() + 1;
          DebugMacro_abv(1, "Oscillation. Count="<<oscillation_count<<'\n' );

        } else {
          if ( prev_status->oscillation_count() > 0 )
            oscillation_count = prev_status->oscillation_count() - 1;
        }
        if ( oscillation_count > 10 ) {
          stagnated = true;
        }
      } else {

        // second situation
        if ( error_diff > 0 )  { // error increases again
          oscillation_count = prev_status->oscillation_count() + 1;
          DebugMacro_abv(1, "Good Oscillation. Count="<<oscillation_count<<'\n' );
        }

        // check for oscillation, call convergence
        if ( oscillation_count > 3 )
          converged = true;
      }
    }

  } else {
    converged = false;
  }

  return new rgrl_converge_status( converged, stagnated, good_enough, false, new_error, oscillation_count, error_diff );
}
