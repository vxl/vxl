//:
// \file
// \author Charlene Tsai
// \date   Feb 2004
//

#include "rgrl_data_manager.h"

#include "rgrl_matcher_k_nearest.h"
#include "rgrl_scale_est_all_weights.h"
#include "rgrl_scale_est_closest.h"
#include "rgrl_weighter_m_est.h"
#include "rgrl_convergence_on_median_error.h"

#include <rrel/rrel_lms_obj.h>
#include <rrel/rrel_tukey_obj.h>

#include <vcl_cassert.h>

// ---------------------------------------------------------------------------
//                                                  data manager
//

rgrl_data_manager::
rgrl_data_manager( bool multi_stage )
  : multi_stage_( multi_stage ),
    multi_feature_( false )
{
}


rgrl_data_manager::
~rgrl_data_manager()
{
}

//: For multi-stage
void 
rgrl_data_manager::
add_data( unsigned stage,
          rgrl_feature_set_sptr                    from_set,
          rgrl_feature_set_sptr                    to_set,
          rgrl_matcher_sptr                        matcher,
          rgrl_weighter_sptr                       weighter,
          rgrl_scale_estimator_unwgted_sptr        unwgted_scale_est,
          rgrl_scale_estimator_wgted_sptr          wgted_scale_est,
          const vcl_string&                        label )
{
  // Both feature sets are mandatory
  assert( from_set && to_set );

  if ( data_.has( stage ) ) multi_feature_ = true;

  // Set other components to defaults if not given
  generate_defaults( matcher, weighter, unwgted_scale_est );

  // Add the data to the data manager
  data_[stage].push_back( rgrl_data_manager_data_item( from_set, to_set, 
                                                       matcher, weighter,
                                                       unwgted_scale_est, 
                                                       wgted_scale_est, 
                                                       label ) );
  data_.set_dimension_increase_for_next_stage(stage, 1);
}

//: For single-stage 
void 
rgrl_data_manager::
add_data( rgrl_feature_set_sptr                    from_set,
          rgrl_feature_set_sptr                    to_set,
          rgrl_matcher_sptr                        matcher,
          rgrl_weighter_sptr                       weighter,
          rgrl_scale_estimator_unwgted_sptr        unwgted_scale_est,
          rgrl_scale_estimator_wgted_sptr          wgted_scale_est,
          const vcl_string&                        label )
{
  assert( !multi_stage_ );
  
  unsigned stage = 0;

  add_data( stage, from_set, to_set, matcher, weighter,
            unwgted_scale_est, wgted_scale_est, label );
}

void 
rgrl_data_manager::
add_estimator( unsigned                           stage, 
               rgrl_estimator_sptr                estimator)
{
  data_.add_estimator(stage, estimator);
}

//: For single-stage
void 
rgrl_data_manager::
add_estimator( rgrl_estimator_sptr                estimator)
{
  assert( !multi_stage_ );

  unsigned stage = 0;
  data_.add_estimator(stage, estimator);
}

//: Operations for dim_increase_for_next_stage for multi-stage
void 
rgrl_data_manager::
set_dimension_increase_for_next_stage( unsigned stage, double rate)
{
  data_.set_dimension_increase_for_next_stage(stage, rate);
}
  
double 
rgrl_data_manager::
dimension_increase_for_next_stage(unsigned stage) const
{
  return data_.dimension_increase_for_next_stage( stage );
}

//: Check if any stage contains multi-type
bool 
rgrl_data_manager::
is_multi_feature() const
{
  return multi_feature_;
}

//: For multi-stage, multi-type
void
rgrl_data_manager::
get_data_at_stage( unsigned stage,
                   vcl_vector<rgrl_feature_set_sptr>             & from_sets,
                   vcl_vector<rgrl_feature_set_sptr>             & to_sets,
                   vcl_vector<rgrl_matcher_sptr>                 & matchers,
                   vcl_vector<rgrl_weighter_sptr>                & weighters,
                   vcl_vector<rgrl_scale_estimator_unwgted_sptr> & unwgted_scale_ests,
                   vcl_vector<rgrl_scale_estimator_wgted_sptr>   & wgted_scale_ests,
                   vcl_vector<rgrl_estimator_sptr>               & estimators) const
{
  from_sets.clear();
  to_sets.clear();
  matchers.clear();
  weighters.clear();
  unwgted_scale_ests.clear();
  wgted_scale_ests.clear();
  estimators.clear();

  if( data_.has( stage ) ) {
    typedef rgrl_data_manager_data_storage::data_vector::const_iterator iter_type;
    iter_type itr = data_[stage].begin();
    iter_type end = data_[stage].end();
    for( ; itr != end; ++itr ) {
      from_sets.push_back( itr->from_set );
      to_sets.push_back( itr->to_set );
      matchers.push_back( itr->matcher );
      unwgted_scale_ests.push_back( itr->unwgted_scale_est );
      wgted_scale_ests.push_back( itr->wgted_scale_est );
      weighters.push_back( itr->weighter );
    }

    if ( data_.has_estimator_hierarchy( stage ) )
      estimators = data_.estimator_hierarchy( stage ); 
  }
}

//: For multi-stage, single-type
void
rgrl_data_manager::
get_data_at_stage( unsigned stage,
                   rgrl_feature_set_sptr             & from_set,
                   rgrl_feature_set_sptr             & to_set,
                   rgrl_matcher_sptr                 & matcher,
                   rgrl_weighter_sptr                & weighter,
                   rgrl_scale_estimator_unwgted_sptr & unwgted_scale_est,
                   rgrl_scale_estimator_wgted_sptr   & wgted_scale_est,
                   vcl_vector<rgrl_estimator_sptr>   & estimators ) const 
{
  assert( !multi_feature_ );

  vcl_vector<rgrl_feature_set_sptr>             from_sets;
  vcl_vector<rgrl_feature_set_sptr>             to_sets;
  vcl_vector<rgrl_matcher_sptr>                 matchers;
  vcl_vector<rgrl_weighter_sptr>                weighters;
  vcl_vector<rgrl_scale_estimator_unwgted_sptr> unwgted_scale_ests;
  vcl_vector<rgrl_scale_estimator_wgted_sptr>   wgted_scale_ests;

  rgrl_data_manager::get_data_at_stage( stage,
                                        from_sets,
                                        to_sets,
                                        matchers,
                                        weighters,
                                        unwgted_scale_ests,
                                        wgted_scale_ests,
                                        estimators );
  
  from_set = from_sets[0];
  to_set   = to_sets[0];
  matcher  = matchers[0];
  unwgted_scale_est = unwgted_scale_ests[0];
  wgted_scale_est = wgted_scale_ests[0];
  weighter = weighters[0];
}

//: For single-stage, multi-type
void
rgrl_data_manager::
get_data( vcl_vector<rgrl_feature_set_sptr>             & from_sets,
          vcl_vector<rgrl_feature_set_sptr>             & to_sets,
          vcl_vector<rgrl_matcher_sptr>                 & matchers,
          vcl_vector<rgrl_weighter_sptr>                & weighters,
          vcl_vector<rgrl_scale_estimator_unwgted_sptr> & unwgted_scale_ests,
          vcl_vector<rgrl_scale_estimator_wgted_sptr>   & wgted_scale_ests,
          vcl_vector<rgrl_estimator_sptr>               & estimators) const
{
  assert( !multi_stage_ );

  unsigned stage = 0;
  get_data_at_stage( stage, from_sets, to_sets, matchers, weighters,
                     unwgted_scale_ests, wgted_scale_ests, estimators );
}

//: For single-stage, single-type
void
rgrl_data_manager::
get_data( rgrl_feature_set_sptr             & from_set,
          rgrl_feature_set_sptr             & to_set,
          rgrl_matcher_sptr                 & matcher,
          rgrl_weighter_sptr                & weighter,
          rgrl_scale_estimator_unwgted_sptr & unwgted_scale_est,
          rgrl_scale_estimator_wgted_sptr   & wgted_scale_est,
          vcl_vector<rgrl_estimator_sptr>   & estimators ) const
{
  assert( !multi_stage_ );

  unsigned stage = 0;
  get_data_at_stage( stage, from_set, to_set, matcher, weighter,
                     unwgted_scale_est, wgted_scale_est, estimators );
}

//: Check if certain stage exists with data
bool 
rgrl_data_manager::
has_stage(unsigned stage ) const
{
  return data_.has( stage );
}

void
rgrl_data_manager::
generate_defaults(  rgrl_matcher_sptr                  &matcher,
                    rgrl_weighter_sptr                 &weighter,
                    rgrl_scale_estimator_unwgted_sptr  &unwgted_scale_est )
{
  // If matcher not set, set matcher to ICP
  if ( !matcher ) {
    matcher = new rgrl_matcher_k_nearest( 1 );
    DebugMacro( 1, "Default matcher set to rgrl_matcher_k_nearest( 1 ) \n"); 
  }

  // If robust_registration, set missing components to default
  //
  // weighter:
  if ( !weighter ) {
    vcl_auto_ptr<rrel_m_est_obj>  m_est_obj( new rrel_tukey_obj(4) );
    weighter = new rgrl_weighter_m_est(m_est_obj, false, false);
    DebugMacro( 1, "Default weighter set to rgrl_weighter_m_est \n"); 
  }

  // unweighted scale estimator:
  if ( !unwgted_scale_est ) {
    vcl_auto_ptr<rrel_objective> lms_obj( new rrel_lms_obj(1) );
    unwgted_scale_est = new rgrl_scale_est_closest( lms_obj );
    DebugMacro( 1, "Default unwgted scale estimator set to rgrl_scale_est_closest \n"); 
  }
}

//: For multi-stage, multi-type
void
rgrl_data_manager::
get_label( unsigned stage,
           vcl_vector<vcl_string>& labels) const
{
  labels.clear();
  labels.reserve(10);

  if( data_.has( stage ) ) {
    typedef rgrl_data_manager_data_storage::data_vector::const_iterator iter_type;
    iter_type itr = data_[stage].begin();
    iter_type end = data_[stage].end();
    for( ; itr != end; ++itr ) {
      labels.push_back( itr->label );
    }
  }
}

//: For single-stage, multi-type
void
rgrl_data_manager::
get_label( vcl_vector<vcl_string>& labels) const
{
  assert( !multi_stage_ );

  const unsigned stage = 0;
  get_label( stage, labels );
}


