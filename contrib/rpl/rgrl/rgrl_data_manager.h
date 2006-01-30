#ifndef rgrl_data_manager_h
#define rgrl_data_manager_h
//:
// \file
// \brief Class to represent the collection of feature sets available for use by the registration engine.
// \author Charlene Tsai
// \date   Feb 2004

#include <rgrl/rgrl_estimator_sptr.h>
#include <rgrl/rgrl_feature_set_sptr.h>
#include <rgrl/rgrl_matcher_sptr.h>
#include <rgrl/rgrl_scale_estimator_sptr.h>
#include <rgrl/rgrl_weighter_sptr.h>
#include <rgrl/rgrl_estimator.h>
#include <rgrl/rgrl_feature_set.h>
#include <rgrl/rgrl_matcher.h>
#include <rgrl/rgrl_scale_estimator.h>
#include <rgrl/rgrl_weighter.h>
#include <rgrl/rgrl_object.h>

#include <vcl_vector.h>
#include <vcl_map.h>
#include <vcl_string.h>

// ---------------------------------------------------------------------------
//                                                                   data item
//

//: An object to store the moving_feature_set, its matching fixed_feature_set, and other components
//  (that work on the two feature sets during registration).
//
class rgrl_data_manager_data_item
{
 public:
  rgrl_data_manager_data_item( rgrl_feature_set_sptr              in_from_set,
                               rgrl_feature_set_sptr              in_to_set,
                               rgrl_matcher_sptr                  in_matcher,
                               rgrl_weighter_sptr                 in_weighter,
                               rgrl_scale_estimator_unwgted_sptr  in_unwgted_scale_est,
                               rgrl_scale_estimator_wgted_sptr    in_wgted_scale_est,
                               const vcl_string&                  in_label = vcl_string() )
    : from_set( in_from_set ),
      to_set( in_to_set ),
      matcher( in_matcher ),
      unwgted_scale_est( in_unwgted_scale_est ),
      wgted_scale_est( in_wgted_scale_est ),
      weighter( in_weighter ),
      label( in_label )
    {
    }

  rgrl_feature_set_sptr             from_set;
  rgrl_feature_set_sptr             to_set;
  rgrl_matcher_sptr                 matcher;
  rgrl_scale_estimator_unwgted_sptr unwgted_scale_est;
  rgrl_scale_estimator_wgted_sptr   wgted_scale_est;
  rgrl_weighter_sptr                weighter;
  vcl_string                        label;
};

// ---------------------------------------------------------------------------
//                                                                data storage
//

//: A collection of data in stages, where each stage can store multiple data items and multiple estimators.
//
class rgrl_data_manager_data_storage
{
 public:
  typedef vcl_vector< rgrl_data_manager_data_item >  data_vector;
  typedef vcl_map< unsigned, data_vector >  data_map;
  typedef vcl_vector< rgrl_estimator_sptr >  estimator_vector;
  typedef vcl_map< unsigned, estimator_vector >  estimator_map;

  //: same as std::map[]
  data_vector& operator[]( unsigned i )
  {
    return map_[i];
  }

  //: Assumes the key is in the map.
  data_vector const& operator[]( unsigned i ) const
  {
    return map_.find( i )->second;
  }

  //: true iff the key is in the map.
  bool has(unsigned i ) const
  {
    return map_.find( i ) != map_.end();
  }

  //:
  void add_estimator( unsigned i, rgrl_estimator_sptr estimator)
  {
    estimators_[i].push_back( estimator );
  }

  //: true iff the key is in the map
  bool has_estimator_hierarchy(unsigned i ) const
  {
    return estimators_.find( i ) != estimators_.end();
  }

  //: Assume the key in in the map
  estimator_vector const& estimator_hierarchy(unsigned i) const
  {
    return estimators_.find( i )->second;
  }

  //: Set dimension increase to go from the current stage to the next
  void set_dimension_increase_for_next_stage( unsigned i, double rate)
  {
    dim_increase_for_next_stage_[i] = rate;
  }

  //: Get dim_increase_for_next_stage
  double dimension_increase_for_next_stage(unsigned i) const
  {
    return dim_increase_for_next_stage_.find( i )->second;
  }

 private:
  data_map map_;
  estimator_map estimators_;
  vcl_map< unsigned, double >  dim_increase_for_next_stage_;
};

// ---------------------------------------------------------------------------
//                                                  data manager
//

class rgrl_data_manager: public rgrl_object
{
 public:
  //: Constructor
  //
  //  If multi_stage set to true, the data manager allows data stored
  //  in multiple stages. The registration engine processes data from
  //  the highest stage to the 0th stage. In the framework of
  //  multi-resolution, the stage index i corresponds to an effective
  //  sigma value of 2^i.
  rgrl_data_manager( bool multi_stage = false );

  ~rgrl_data_manager();

  //:  Add a data item to a multi-stage data storage.
  //
  //  The data item for registration takes at least two feature
  //  sets. Other components are set to the techniques commonly used
  //  in the literature, if not given. In such case, best performance
  //  is not guaranteed. To optimize the registration results, all
  //  components should be specified.
  //
  //  The matcher is ICP, the weighter is m_estimator (Beaton-Tukey),
  //  and the unwgted_scale_estimator is MAD (Median Absolute
  //  Deviation).  wgted_scale_est keeps the original value, since it
  //  is not an essential component.
  //
  void add_data( unsigned stage,
                 rgrl_feature_set_sptr                    from_set,
                 rgrl_feature_set_sptr                    to_set,
                 rgrl_matcher_sptr                        matcher = 0,
                 rgrl_weighter_sptr                       weighter = 0,
                 rgrl_scale_estimator_unwgted_sptr        unwgted_scale_est = 0,
                 rgrl_scale_estimator_wgted_sptr          wgted_scale_est = 0,
                 const vcl_string&                        label = vcl_string() );

  //: Add a data item to a single-stage data storage.
  //
  //  The stage is assumed to be 0.
  //
  void add_data( rgrl_feature_set_sptr                    from_set,
                 rgrl_feature_set_sptr                    to_set,
                 rgrl_matcher_sptr                        matcher = 0,
                 rgrl_weighter_sptr                       weighter = 0,
                 rgrl_scale_estimator_unwgted_sptr        unwgted_scale_est = 0,
                 rgrl_scale_estimator_wgted_sptr          wgted_scale_est = 0,
                 const vcl_string&                        label = vcl_string() );

  //: Add an estimator to a multi-stage data storage.
  void add_estimator( unsigned                           stage,
                      rgrl_estimator_sptr                estimator);

  //: Add an estimator to a single-stage data storage.
  //
  //  The stage is assumed to be 0
  //
  void add_estimator( rgrl_estimator_sptr                estimator);

  //: Set the dimension increase to go from the current stage to the next
  //
  //  If the function is never called for stage \a stage, the
  //  dimension increase for \a stage is 1.
  void set_dimension_increase_for_next_stage( unsigned stage, double rate);

  //: Return \a dimension_increase_for_next_stage_ at stage \a stage
  double dimension_increase_for_next_stage(unsigned stage) const;

  //: Return true if there exists at least one stage containing multiple data_items
  //
  bool is_multi_feature() const;

  //: Return the set of data_items and estimators from a multi-stage data storage.
  //
  void
  get_data_at_stage( unsigned stage,
                     vcl_vector<rgrl_feature_set_sptr>             & from_sets,
                     vcl_vector<rgrl_feature_set_sptr>             & to_sets,
                     vcl_vector<rgrl_matcher_sptr>                 & matchers,
                     vcl_vector<rgrl_weighter_sptr>                & weighters,
                     vcl_vector<rgrl_scale_estimator_unwgted_sptr> & unwgted_scale_ests,
                     vcl_vector<rgrl_scale_estimator_wgted_sptr>   & wgted_scale_ests,
                     vcl_vector<rgrl_estimator_sptr>               & estimators) const;

  //: Return the a single data_item and the set of estimators from a multi-stage data storage.
  //
  void
  get_data_at_stage( unsigned stage,
                     rgrl_feature_set_sptr             & from_sets,
                     rgrl_feature_set_sptr             & to_sets,
                     rgrl_matcher_sptr                 & matchers,
                     rgrl_weighter_sptr                & weighters,
                     rgrl_scale_estimator_unwgted_sptr & unwgted_scale_ests,
                     rgrl_scale_estimator_wgted_sptr   & wgted_scale_ests,
                     vcl_vector<rgrl_estimator_sptr>   & estimators) const;

  //: Return the set of data_items and estimators from a single-stage data storage.
  //
  //  The stage is assumed to be 0.
  void
  get_data( vcl_vector<rgrl_feature_set_sptr>             & from_sets,
            vcl_vector<rgrl_feature_set_sptr>             & to_sets,
            vcl_vector<rgrl_matcher_sptr>                 & matchers,
            vcl_vector<rgrl_weighter_sptr>                & weighters,
            vcl_vector<rgrl_scale_estimator_unwgted_sptr> & unwgted_scale_ests,
            vcl_vector<rgrl_scale_estimator_wgted_sptr>   & wgted_scale_ests,
            vcl_vector<rgrl_estimator_sptr>               & estimators) const;

  //: Return the a single data_item and the set of estimators from a single-stage data storage.
  //
  //  The stage is assumed to be 0.
  void
  get_data( rgrl_feature_set_sptr             & from_sets,
            rgrl_feature_set_sptr             & to_sets,
            rgrl_matcher_sptr                 & matchers,
            rgrl_weighter_sptr                & weighters,
            rgrl_scale_estimator_unwgted_sptr & unwgted_scale_ests,
            rgrl_scale_estimator_wgted_sptr   & wgted_scale_ests,
            vcl_vector<rgrl_estimator_sptr>   & estimators) const;

  //: Return labels
  void get_label( unsigned stage, vcl_vector<vcl_string>& labels ) const;

  //: Return labels
  void get_label( vcl_vector<vcl_string>& labels ) const;

  //: Return true if certain stage exists with data
  bool has_stage(unsigned i ) const;

  // Defines type-related functions
  rgrl_type_macro( rgrl_data_manager, rgrl_object );

 private:
  void generate_defaults(rgrl_matcher_sptr&                  matcher,
                         rgrl_weighter_sptr&                 weighter,
                         rgrl_scale_estimator_unwgted_sptr&  unwgted_scale_est );

 private:
  rgrl_data_manager_data_storage   data_;
  bool                             multi_stage_;
  bool                             multi_feature_;
};

#endif
