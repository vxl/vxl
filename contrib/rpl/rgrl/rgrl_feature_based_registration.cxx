#include "rgrl_feature_based_registration.h"
//:
// \file
#include "rgrl_initializer.h"
#include "rgrl_convergence_tester.h"
#include "rgrl_converge_status.h"
#include "rgrl_estimator.h"
#include "rgrl_scale_estimator.h"
#include "rgrl_weighter_unit.h"
#include "rgrl_matcher.h"
#include "rgrl_match_set.h"
#include "rgrl_scale_est_null.h"
#include "rgrl_cast.h"
#include "rgrl_data_manager.h"
#include "rgrl_scale.h"
#include "rgrl_util.h"
#include "rgrl_convergence_on_median_error.h"
#include "rgrl_event.h"
#include <vcl_cassert.h>

rgrl_feature_based_registration::
rgrl_feature_based_registration( rgrl_data_manager_sptr data,
                                 rgrl_convergence_tester_sptr conv_tester )
  :data_( data ),
   conv_tester_( conv_tester ),
   num_xforms_tested_( 0 ),
   max_icp_iter_(25),
   expected_max_geometric_scale_ (0),
   expected_min_geometric_scale_ (0),
   iterations_for_scale_est_(-1),
   should_penalize_scaling_(false),
   current_stage_( 0 )
{
}

rgrl_feature_based_registration::
rgrl_feature_based_registration( rgrl_data_manager_sptr data )
  :data_( data ),
   num_xforms_tested_( 0 ),
   max_icp_iter_(25),
   expected_max_geometric_scale_ (0),
   expected_min_geometric_scale_ (0),
   iterations_for_scale_est_(-1),
   should_penalize_scaling_(false)
{
  double tolerance = 1.0;
  conv_tester_ = new rgrl_convergence_on_median_error( tolerance );
}

rgrl_feature_based_registration::
~rgrl_feature_based_registration()
{
}

//: Running from multiple initial estimates, produced by the initializer during registration.
//  Loop through the set of initial estimates, and call the next \a run(.) in the loop.
void
rgrl_feature_based_registration::
run( rgrl_initializer_sptr initializer )
{
  //  Clear previous results
  num_xforms_tested_ = 0;
  best_xform_estimate_ = 0;
  best_matches_.clear();
  best_scales_.clear();
  best_status_ = 0;

  rgrl_transformation_sptr          init_xform_estimate;
  rgrl_scale_sptr                   prior_scale;
  rgrl_estimator_sptr               init_xform_estimator;
  rgrl_mask_box                     image_region(0);
  unsigned                          init_resolution;

  while ( initializer->next_initial( image_region, init_xform_estimator,
                                    init_xform_estimate, init_resolution,
                                    prior_scale ) ) {
    DebugMacro(  1, "Try "<< num_xforms_tested_ << " initial estimate\n" );
    this->run( image_region, init_xform_estimator, init_xform_estimate,
               prior_scale, init_resolution );

    if ( best_status_ && best_status_->is_good_enough() ) break;

    ++num_xforms_tested_;
  }
}

//: Running from a given initial estimate.
//
//  Based on if data_->is_multi_feature(), call run_single_feature(.)
//  or run_multi_feature(.)
//
void
rgrl_feature_based_registration::
run( rgrl_mask_box              image_region,
     rgrl_estimator_sptr        init_xform_estimator,
     rgrl_transformation_sptr   initial_xform,
     rgrl_scale_sptr            prior_scale,
     unsigned                   init_resolution)
{
  if ( data_->is_multi_feature() ) {
    DebugMacro(1, " Multi-feature Registration:\n");
    this->register_multi_feature(image_region, init_xform_estimator,
                                 initial_xform, prior_scale, init_resolution);
  }
  else {
    DebugMacro(  1, " Single-feature Registration:\n" );

    this->register_single_feature(image_region, init_xform_estimator,
                                  initial_xform, prior_scale, init_resolution);
  }
}

//////////////// functions to access internal data  ////////////////////////

//:  Return the final, best estimate
rgrl_transformation_sptr
rgrl_feature_based_registration::
final_transformation() const
{
  return best_xform_estimate_;
}

//:  Return the scales of the best transformation estimate
rgrl_set_of<rgrl_scale_sptr> const&
rgrl_feature_based_registration::
final_scales() const
{
  return best_scales_;
}

rgrl_scale_sptr
rgrl_feature_based_registration::
final_scale() const
{
  assert ( !data_->is_multi_feature() );
  return best_scales_[0];
}

//:  Return the status of the best transformation estimate.
rgrl_converge_status_sptr
rgrl_feature_based_registration::
final_status() const
{
  return best_status_;
}

//:  The matches used for the best transformation estimate.
rgrl_set_of<rgrl_match_set_sptr>  const&
rgrl_feature_based_registration::
final_match_sets() const
{
  return best_matches_;
}

rgrl_match_set_sptr
rgrl_feature_based_registration::
final_match_set() const
{
  assert ( !data_->is_multi_feature() );
  return best_matches_[0];
}

//:  Return the number of initial transformations tested
unsigned
rgrl_feature_based_registration::
num_initial_xforms_tested() const
{
  return num_xforms_tested_;
}

//:  Return true is has a best xform_estimate
bool
rgrl_feature_based_registration::
has_final_transformation() const
{
  if ( best_xform_estimate_ ) return true;
  return false;
}

//: Set the max number of icp iteration per level
void
rgrl_feature_based_registration::
set_max_icp_iter( unsigned iter )
{
   max_icp_iter_ = iter;
}

//: Set the expected max scale
void
rgrl_feature_based_registration::
set_expected_max_geometric_scale( double scale)
{
  expected_max_geometric_scale_ = scale;
}

//: Set the expected min scale
void
rgrl_feature_based_registration::
set_expected_min_geometric_scale( double scale)
{
  expected_min_geometric_scale_ = scale;
}

//: Set the number of iteration for scale estimation
void
rgrl_feature_based_registration::
set_iterations_for_scale_est( int iter)
{
  iterations_for_scale_est_ = iter;
}

//: penalize transformation that involves scaling of the registraion area
void
rgrl_feature_based_registration::
 penalize_scaling( bool penalize)
{
  should_penalize_scaling_ = penalize;
}


//: Return the current match sets
rgrl_set_of<rgrl_match_set_sptr>  const&
rgrl_feature_based_registration::
current_match_sets() const
{
  return current_match_sets_;
}

//:  Return the current estimate
rgrl_transformation_sptr
rgrl_feature_based_registration::
current_transformation() const
{
  return current_xform_estimate_;
}

//:  Return the current stage
unsigned
rgrl_feature_based_registration::
current_stage() const
{
  return current_stage_;
}

//:  Return the iteration at current stage
unsigned
rgrl_feature_based_registration::
iterations_at_current_stage() const
{
  return iterations_at_stage_;
}


//////////////////// private functions //////////////////////////

//: registration of single feature type at each stage/resolution
void
rgrl_feature_based_registration::
register_single_feature( rgrl_mask_box            image_region,
                         rgrl_estimator_sptr      initial_xform_estimator,
                         rgrl_transformation_sptr xform_estimate,
                         rgrl_scale_sptr          scale,
                         unsigned                 resolution )
{
  rgrl_converge_status_sptr         current_status;
  rgrl_feature_set_sptr             from_set;
  rgrl_feature_set_sptr             to_set;
  rgrl_matcher_sptr                 matcher;
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr   wgted_scale_est;
  rgrl_weighter_sptr                weighter;
  rgrl_match_set_sptr               match_set;
  vcl_vector<rgrl_estimator_sptr>   xform_estimators;
  rgrl_estimator_sptr               xform_estimator;
  bool                              failed, scale_in_range;
  unsigned                          prev_resol;

  failed = false;
  scale_in_range = true;
  current_xform_estimate_ = xform_estimate;

  assert ( data_->has_stage( resolution ) );

  do { // for each stage/resolution
    data_->get_data_at_stage( resolution, from_set, to_set, matcher, weighter,
                              unwgted_scale_est, wgted_scale_est, xform_estimators);
    match_set = 0;
    current_stage_ = resolution;

    DebugMacro(  1, " Current resolution "<< resolution <<'\n' );
    DebugMacro( 2, "Current geometric scale = "<<scale->geometric_scale()<<'\n' );

    // If no estimator found for the current stage, the default is
    // from the initializer. Feature_based can only deal with one
    // estimator in each stage/resolution. If more than one is found,
    // only the first is kept.
    //
    if ( xform_estimators.empty() )
      xform_estimator = initial_xform_estimator;
    else xform_estimator = xform_estimators[0];
    assert ( xform_estimator );

    iterations_at_stage_ = 0; //keeps track of total iter at stage
    current_status = 0;
    bool should_estimate_scale = true;
    int  scale_est_count = 0;

    do { // for each re-match

      DebugMacro(  2, " Computing matches and scales\n" );
      // Compute matches, and scales for each feature set.
      //
      match_set= matcher->compute_matches( *from_set,
                                           *to_set,
                                           *current_xform_estimate_,
                                           image_region,
                                           *scale );
      current_match_sets_.clear();
      current_match_sets_.push_back( match_set );
      DebugMacro(  2, "      Matches: " << match_set->from_size() <<'\n' );

      // For the first iteration, use prior scale or estimate the
      // scales without weights (since we don't have any...)
      //
      if ( iterations_for_scale_est_ >= 0 &&
           scale_est_count > iterations_for_scale_est_ ) {
        should_estimate_scale = false;
      }

      rgrl_scale_sptr  new_scale = 0;
      if ( !should_estimate_scale ) {
        DebugMacro(  2, "No scale estimation\n" );
      }
      else if ( match_set->from_size() == 0 ) {
        DebugMacro(0, "rgrl_feature_based_registration:: Empty match set!!!\n");
        failed = true;
        continue;
      }
      else { //should_estimate_scale && match_set->from_size() > 0
        if ( !wgted_scale_est ) {
          new_scale = unwgted_scale_est->
            estimate_unweighted( *match_set, scale, should_penalize_scaling_ );
        } else {
          if ( iterations_at_stage_ == 0 && !scale ) {
            assert ( unwgted_scale_est );
            new_scale = unwgted_scale_est->
              estimate_unweighted( *match_set, scale, should_penalize_scaling_);
          }
          else {
            weighter->compute_weights(*scale, *match_set);
            new_scale = wgted_scale_est->
              estimate_weighted( *match_set, scale, should_penalize_scaling_);
          }
        }

        // If the new scale exists, and the geometric scale is above
        // the lower bound, replace the old one by the new one
        if ( new_scale ) {
          DebugMacro( 2, "New geometric scale = "<<new_scale->geometric_scale()<<'\n' );
          if ( new_scale->has_geometric_scale() &&
               new_scale->geometric_scale() < expected_min_geometric_scale_ ) {
            should_estimate_scale = false;
            scale->set_geometric_scale(expected_min_geometric_scale_);
            DebugMacro( 2, "Scale below expected_min_geometric_scale. Set to expected_min_geometric_scale.\n" );
          }
          else {
            scale = new_scale;
          }
          scale_est_count++;
        }
      }
      DebugMacro( 2, "Current geometric scale = "<<scale->geometric_scale()<<'\n' );
      assert ( scale );

      // If the scale is above the upper bound of the expected
      // geometric scale return with failure
      if ( expected_max_geometric_scale_ > 0 &&
           scale->has_geometric_scale() &&
           scale->geometric_scale() > expected_max_geometric_scale_) {
        scale_in_range = false;
        failed = true;
        continue;
      }

      DebugMacro(  2, " Estimate the transformation\n" );

      // Transformation estimation using a simplified irls
      // (Iterative-Reweighted Least-Squares) routine.
      //
      // The match sets and the scales are fixed, but the associated
      // weights are updated (hence reweighted-least-squares)
      // throughout the estimation.
      //
      double alignment_error;
      if ( !rgrl_util_irls( match_set, scale, weighter,
                            *conv_tester_, xform_estimator,
                            current_xform_estimate_, alignment_error,
                            this->debug_flag() ) ) {
        failed = true;
        continue; //no valid xform, so exit the loop
      }

      // For debugging
      //
      this->invoke_event(rgrl_event_iteration());

      // Update the weights and scale estimates based on the new transform
      //
      DebugMacro(  2, " Updating scale estimates and checking for validity\n" );

      match_set->remap_from_features( *current_xform_estimate_ );
      weighter->compute_weights( *scale, *match_set );

#if 0
      // CT: this step seems redundant, since the scale is re-computed
      // at the beginning of next iteration.

      if ( should_estimate_scale ) {
        if ( !wgted_scale_est )
          scale = unwgted_scale_est->
            estimate_unweighted( *match_set, scale, should_penalize_scaling_ );
        else
          scale = wgted_scale_est->
            estimate_weighted( *match_set, scale, should_penalize_scaling_ );
        DebugMacro(  2, " Geometric scale after converged = "<< scale->geometric_scale()<<'\n' );
      }
#endif // 0

      // Perform convergence test
      //
      DebugMacro(  2, " Perform convergence test\n" );
      current_status =
        conv_tester_->compute_status( current_status,
                                      current_xform_estimate_,
                                      xform_estimator,
                                      match_set, scale,
                                      should_penalize_scaling_);
      DebugMacro(  3, "run: (iterations = " << iterations_at_stage_
                   << ") oscillation count = " << current_status->oscillation_count() << '\n' );
      DebugMacro(  3, "run: error = " << current_status->error() << vcl_endl );
      DebugMacro(  3, "run: error_diff = " << current_status->error_diff() << vcl_endl );

      ++iterations_at_stage_;
    } while ( !failed &&
             !current_status->has_converged() &&
             !current_status->has_stagnated() &&
             iterations_at_stage_ < max_icp_iter_ );

    if ( failed ) {
      if ( !scale_in_range )
        DebugMacro(  1, " Geometric scale above the expected value\n" );
      else
        DebugMacro( 1, " Failed with empty match set, or irls estimation\n" );
      continue;
    }
    if ( current_status->has_converged() )
      DebugMacro(  1, " CONVERGED\n" );
    if ( current_status->has_stagnated() )
      DebugMacro(  1, " STAGNATED\n" );
    if ( iterations_at_stage_ == max_icp_iter_ )
      DebugMacro(  1, " ICP iteration reached maximum ("<<max_icp_iter_<<" )\n" );

    // Move to the next resolution with proper initialization if not
    // at the finest level
    //
    prev_resol = resolution;
    initialize_for_next_resolution( image_region, current_xform_estimate_,
                                    resolution );
    int level_diff = prev_resol - resolution;
    double dim_increase = data_->dimension_increase_for_next_stage(prev_resol);
//  double scale_multipler = vcl_pow(dim_increase, level_diff);
    scale->set_geometric_scale( scale->geometric_scale()*
                                vcl_pow(dim_increase, level_diff) );

  } while ( !failed &&
            ( !resolution == 0 ||
              !prev_resol == 0 ) );

  DebugMacro( 1, "Estimation complete\n" );

  //   At this point the iterations are over for this initial
  //   estimate.  If the estimate is successful, if it is the first
  //   one tested or it is the best seen thus far, record information
  //   about this estimate.
  //

  if ( !failed ){
    DebugMacro( 1, "Obj value after convergence = "<<current_status->objective_value()<<'\n');
    if ( !best_status_ ||
         current_status->objective_value() < best_status_->objective_value() ) {
      best_xform_estimate_ = current_xform_estimate_;
      if (best_scales_.size() != 1)
        best_scales_.resize(1);
      best_scales_[0] = scale;
      if (best_matches_.size() != 1)
        best_matches_.resize(1);
      best_matches_[0] = match_set;
      best_status_ = current_status;
      DebugMacro( 1, "Set best xform estimate\n" );
    }
  }
}

//: registration of multiple feature type at each stage/resolution
void
rgrl_feature_based_registration::
register_multi_feature( rgrl_mask_box            image_region,
                        rgrl_estimator_sptr      initial_xform_estimator,
                        rgrl_transformation_sptr xform_estimate,
                        rgrl_scale_sptr          prior_scale,
                        unsigned                 resolution )
{
  rgrl_converge_status_sptr                     current_status;
  vcl_vector<rgrl_feature_set_sptr>             from_sets;
  vcl_vector<rgrl_feature_set_sptr>             to_sets;
  vcl_vector<rgrl_matcher_sptr>                 matchers;
  vcl_vector<rgrl_scale_estimator_unwgted_sptr> unwgted_scale_ests;
  vcl_vector<rgrl_scale_estimator_wgted_sptr>   wgted_scale_ests;
  vcl_vector<rgrl_weighter_sptr>                weighters;
  rgrl_set_of<rgrl_scale_sptr>                  scales;
  vcl_vector<rgrl_estimator_sptr>               xform_estimators;
  rgrl_estimator_sptr                           xform_estimator;
  bool                                          failed, scale_in_range, use_prior_scale;
  unsigned                                      prev_resol;

  use_prior_scale = false;
  if ( prior_scale ) use_prior_scale = true;
  scale_in_range = true;
  failed = false;
  current_xform_estimate_ = xform_estimate;

  assert ( data_->has_stage( resolution ) );

  do { // for each stage/resolution
    data_->get_data_at_stage( resolution, from_sets, to_sets, matchers,
                              weighters, unwgted_scale_ests,
                              wgted_scale_ests, xform_estimators);
    current_stage_ = resolution;
    unsigned data_count = from_sets.size();

    DebugMacro(  1, " Current resolution "<< resolution <<'\n' );

    // Initialize the size of match_sets and scales to be the same as
    // the from_sets
    //
    current_match_sets_.clear();
    scales.clear();
    current_match_sets_.resize( data_count );
    scales.resize( data_count );

    // If no estimator found for the current stage, the default is
    // from the initializer. Feature_based can only deal with one
    // estimator in each stage/resolution. If more than one is found,
    // only the first is kept.
    //
    if ( xform_estimators.size() == 0 )
      xform_estimator = initial_xform_estimator;
    else xform_estimator = xform_estimators[0];
    assert ( xform_estimator );

    // If the initialization comes with a prior scale, initialize the
    // scales using the prior scale.
    //
    if ( use_prior_scale ) {
      DebugMacro(  2, "Prior scale = "<<prior_scale->geometric_scale()<<'\n' );
      for (  unsigned int fs = 0; fs < data_count; ++fs )
        scales[fs] = prior_scale;
    }

    iterations_at_stage_ = 0; //keeps track of total iter at level
    current_status = 0;
    bool should_estimate_scale = true;
    int  scale_est_count = 0;

    do { // for each re-matching

      DebugMacro(  2, " Computing matches and scales\n" );
      // Compute matches, and scales for each feature set.
      //
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        DebugMacro(  2, "   Data set " << fs << vcl_endl );
        rgrl_match_set_sptr new_matches =
          matchers[fs]->compute_matches( *from_sets[fs],
                                         *to_sets[fs],
                                         *current_xform_estimate_,
                                         image_region,
                                         *scales[fs] );
        DebugMacro(  2, "      Matches: " << new_matches->from_size() <<" ("<<new_matches<<")\n" );

        // For the first iteration, use prior scale or estimate the
        // scales without weights (since we don't have any...)
        //
        if ( iterations_for_scale_est_ >= 0 &&
             scale_est_count > iterations_for_scale_est_ ) {
          should_estimate_scale = false;
        }

        rgrl_scale_sptr new_scale = 0;
        if ( !should_estimate_scale ) {
          DebugMacro(  2, "No scale estimation\n" );
        }
        else if ( new_matches->from_size() == 0 ) {
          DebugMacro(0, "Empty match set!!!\n");
          failed = true;
          continue;
        }
        else { //should_estimate_scale && new_matches->from_size() > 0
          if ( !wgted_scale_ests[fs] ) {
            new_scale = unwgted_scale_ests[fs]->
              estimate_unweighted( *new_matches, scales[fs], should_penalize_scaling_ );
          } else {
            if ( iterations_at_stage_ == 0 && !scales[fs] ) {
              assert ( unwgted_scale_ests[fs] );
              new_scale = unwgted_scale_ests[fs]->
                estimate_unweighted( *new_matches, scales[fs], should_penalize_scaling_);
            }
            else {
              weighters[fs]->compute_weights(*scales[fs], *new_matches);
              new_scale = wgted_scale_ests[fs]->
                estimate_weighted( *new_matches, scales[fs], should_penalize_scaling_);
            }
          }

          if ( new_scale ) {
            DebugMacro(  2, "New geometric scale = "<<new_scale->geometric_scale()<<'\n' );
            if ( new_scale->has_geometric_scale() &&
                 new_scale->geometric_scale() < expected_min_geometric_scale_ ) {
              should_estimate_scale = false;
              scales[fs]->set_geometric_scale(expected_min_geometric_scale_);
              DebugMacro( 2, "Scale below expected_min_geometric_scale. Set to expected_min_geometric_scale.\n" );
            }
            else {
              scales[fs] = new_scale;
            }
            scale_est_count++;
          }
        }
        assert ( scales[fs] );

        // If the scale is above the upper bound of the expected
        // geometric scale return with failure
        if ( expected_max_geometric_scale_ > 0 &&
             scales[fs]->has_geometric_scale() &&
             scales[fs]->geometric_scale() > expected_max_geometric_scale_) {
          scale_in_range = false;
          failed = true;
        }

        // Keep new ones and discard old ones
        //
        current_match_sets_[fs] = new_matches;
      }

      DebugMacro(  2, " Estimate the transformation\n" );

      // Estimate the transformation using a simplified irls
      // (Iterative-Reweighted Least-Squares) routine.
      //
      // The match sets and the scales are fixed, but the associated
      // weights are updated (hence reweighted-least-squares)
      // throughout the estimation.
      double alignment_error;
      if ( !rgrl_util_irls( current_match_sets_, scales, weighters,
                            *conv_tester_, xform_estimator,
                            current_xform_estimate_, alignment_error,
                            this->debug_flag() ) ) {
        failed = true;
        continue; //no valid xform, so exit the loop
      }

      // For debugging ...
      //
      this->invoke_event(rgrl_event_iteration());

      // Update the weights and scale estimates based on the new transform
      //
      DebugMacro(  2, " Updating scale estimates and checking for validity\n" );
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        if ( current_match_sets_[fs]->from_size() > 0 ) {
          current_match_sets_[fs]->remap_from_features( *current_xform_estimate_ );
          weighters[fs]->compute_weights( *scales[fs], *current_match_sets_[fs] );
#if 0
          // CT: this step seems redundant, since the scale is re-computed
          // at the beginning of next iteration.
          if ( should_estimate_scale ) {
            if ( !wgted_scale_ests[fs] )
              scales[fs] = unwgted_scale_ests[fs]->
                estimate_unweighted( *current_match_sets_[fs], scales[fs], should_penalize_scaling_ );
            else scales[fs] = wgted_scale_ests[fs]->
                   estimate_weighted( *current_match_sets_[fs], scales[fs], should_penalize_scaling_ );
            DebugMacro(  2, " Geometric scale after converged = "<< scales[fs]->geometric_scale()<<'\n' );
          }
#endif // 0
        }
      }

      // Perform convergence test
      //
      DebugMacro(  2, " Perform convergence test\n" );
      current_status =
        conv_tester_->compute_status( current_status,
                                      current_xform_estimate_,
                                      xform_estimator,
                                      current_match_sets_, scales,
                                      should_penalize_scaling_);
      DebugMacro( 3, "run: (iterations = " << iterations_at_stage_
                  << ") oscillation count = " << current_status->oscillation_count() << '\n' );
      DebugMacro( 3, "run: error = " << current_status->error() << vcl_endl );
      DebugMacro( 3, "run: error_diff = " << current_status->error_diff() << vcl_endl );

      ++iterations_at_stage_;
    } while ( !failed &&
             !current_status->has_converged() &&
             !current_status->has_stagnated() &&
             iterations_at_stage_ < max_icp_iter_ );

      if ( failed ) {
        if ( !scale_in_range )
          DebugMacro(  2, " Geometric scale above the expected value\n" );
        else
          DebugMacro(  2, " Failed with empty match set, or feature_based\n" );
        continue;
      }
      if ( current_status->has_converged() )
        DebugMacro( 1, " CONVERGED\n" );
      if ( current_status->has_stagnated() )
        DebugMacro( 1, " STAGNATED\n" );
      if ( iterations_at_stage_ == max_icp_iter_ )
        DebugMacro( 1, " ICP iteration reached maximum ("<<max_icp_iter_<<" )\n" );

      // Move to the next resolution with proper initialization if not
      // already at the finest level
      //
      prev_resol = resolution;
      initialize_for_next_resolution( image_region, current_xform_estimate_,
                                      resolution );
      int level_diff = prev_resol - resolution;
      double dim_increase = data_->dimension_increase_for_next_stage(prev_resol);
      double scale_multipler = vcl_pow(dim_increase, level_diff);
      for ( unsigned int fs=0; fs < data_count; ++fs ) {
        scales[fs]->set_geometric_scale( scales[fs]->geometric_scale()*scale_multipler );
      }

      use_prior_scale = true;
      prior_scale = scales[0]; //Assuming scales[0] is a good approximate

  } while ( !failed &&
            ( !resolution == 0 ||
              !prev_resol == 0) );

  DebugMacro( 1, "Estimation complete\n" );

  //   At this point the iterations are over for this initial
  //   estimate.  If the estimation is successful, and if it is the
  //   first one tested or it is the best seen thus far, record
  //   information about this estimate.
  //

  if ( !failed ){
    DebugMacro( 1, "Obj value after convergence = "<<current_status->objective_value()<<'\n');
    if ( !best_status_ ||
         current_status->objective_value() < best_status_->objective_value() ) {
      best_xform_estimate_ = current_xform_estimate_;
      best_scales_         = scales;
      best_matches_        = current_match_sets_;
      best_status_         = current_status;
      DebugMacro( 1, "Set best xform estimate\n" );
    }
  }
}


void
rgrl_feature_based_registration::
initialize_for_next_resolution(  rgrl_mask_box            & image_region,
                                 rgrl_transformation_sptr & xform_estimate,
                                 unsigned                 & current_resol ) const
{
  // Find the next available level
  //
  unsigned new_resol = current_resol;
  int next_resol = new_resol - 1;
  for ( ; next_resol >=0 ; --next_resol) {
    if ( data_->has_stage(next_resol) ) {
      new_resol = next_resol;
      break;
    }
  }

  // If no next available level return
  //
  if ( new_resol == current_resol ) return;


  // Scale the components of the current view for the initial view of the next
  // available level
  //
  int level_diff = current_resol - new_resol;
  double dim_increase = data_->dimension_increase_for_next_stage(current_resol);
  double scale = vcl_pow(dim_increase, level_diff);

  image_region.set_x0( image_region.x0()*scale );
  image_region.set_x1( image_region.x1()*scale );

  xform_estimate =  xform_estimate->scale_by( scale );
  current_resol = new_resol;
}
