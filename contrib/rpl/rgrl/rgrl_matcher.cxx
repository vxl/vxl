#include <iostream>
#include <algorithm>
#include "rgrl_matcher.h"
//:
// \file
#include <rgrl/rgrl_view.h>
#include <rgrl/rgrl_feature.h>
#include <rgrl/rgrl_match_set.h>
#ifdef _MSC_VER
#  include "vcl_msvc_warnings.h"
#endif

// less than operator
bool
rgrl_matcher::flip_node::
operator<( flip_node const& other ) const
{
  return this->to_.as_pointer() < other.to_.as_pointer();
}

rgrl_matcher::
~rgrl_matcher() = default;

rgrl_match_set_sptr
rgrl_matcher::
compute_matches( rgrl_feature_set const&    from_features,
                 rgrl_feature_set const&    to_features,
                 rgrl_view const&           current_view,
                 rgrl_scale const&          current_scale,
                 rgrl_match_set_sptr const& old_matches )
{
  return compute_matches( from_features, to_features, current_view,
                          *current_view.xform_estimate(), current_scale, old_matches );
}

rgrl_match_set_sptr
rgrl_matcher::
compute_matches( rgrl_feature_set const&    from_features,
                 rgrl_feature_set const&    to_features,
                 rgrl_transformation const& current_xform,
                 rgrl_mask_box const&       from_region,
                 rgrl_mask_box const&       to_region,
                 rgrl_scale const&          current_scale,
                 rgrl_match_set_sptr const& old_matches )
{
  rgrl_mask_sptr from_roi = new rgrl_mask_box( from_region.x0(), from_region.x1() );
  rgrl_mask_sptr to_roi = new rgrl_mask_box( to_region.x0(), to_region.x1() );
  rgrl_view view( from_roi, to_roi, from_region, from_region, nullptr, nullptr, 0);

  return this->compute_matches(from_features,
                               to_features,
                               view,
                               current_xform,
                               current_scale,
                               old_matches);
}

void
rgrl_matcher::
add_one_flipped_match( rgrl_match_set_sptr&      inv_set,
                       rgrl_view          const& current_view,
                       nodes_vec_iterator const& begin_iter,
                       nodes_vec_iterator const& end_iter )
{
  const auto size = unsigned( end_iter - begin_iter );
  rgrl_transformation_sptr const& inverse_xform = current_view.inverse_xform_estimate();

  rgrl_feature_sptr from = begin_iter->to_;
  rgrl_feature_sptr mapped = from->transform( *inverse_xform );

  // setup structure
  std::vector< rgrl_feature_sptr > matching_tos;
  std::vector< double >            sig_wgts;
  matching_tos.reserve( size );
  sig_wgts.reserve( size );

  // copy features
  for ( nodes_vec_iterator itr = begin_iter; itr!=end_iter; ++itr ) {
    matching_tos.push_back( itr->from_ );
    sig_wgts.push_back( itr->sig_wgt_ );
  }

  // add matches
  inv_set->add_feature_matches_and_weights( from, mapped, matching_tos, sig_wgts );
}

// default behavior when inverting a set of matches
rgrl_match_set_sptr
rgrl_matcher::
invert_matches( rgrl_match_set const&    current_set,
                rgrl_view const&         current_view )
{
  typedef rgrl_match_set::const_from_iterator from_iter;
  typedef from_iter::to_iterator        to_iter;

  rgrl_match_set_sptr inv_set
    = new rgrl_match_set( current_set.to_feature_type(),
                          current_set.from_feature_type(),
                          current_set.to_label(),
                          current_set.from_label() );

  inv_set->reserve( 3*current_set.from_size() );
  std::vector< flip_node >  matches;
  matches.reserve( 5*current_set.from_size() );

  flip_node tmp;
  for ( from_iter fitr = current_set.from_begin(); fitr != current_set.from_end(); ++fitr )
  {
    if ( fitr.size() == 0 )  continue;

    for ( to_iter titr = fitr.begin(); titr != fitr.end(); ++titr )
    {
      tmp.from_    = fitr.from_feature();
      tmp.to_      = titr.to_feature();
      tmp.sig_wgt_ = titr.signature_weight();

      matches.push_back( tmp );
      //rgrl_feature_sptr mapped = from->transform( *reverse_xform );
      //std::vector<rgrl_feature_sptr> to_vec(1, to );
      //std::vector<double>            sig_vec(1, titr.signature_weight() );
      //inv_set->add_feature_matches_and_weights( from, mapped, to_vec, sig_vec );
    }
  }

  // empty match set
  if ( matches.empty() )
    return inv_set;

  // sort the matches according to To feature pointer
  std::sort( matches.begin(), matches.end() );

  std::vector<flip_node>::const_iterator begin_iter, end_iter;
  for ( begin_iter=matches.begin(), end_iter=matches.begin()+1;
        end_iter!=matches.end(); ++end_iter ) {
    if ( end_iter->to_ == begin_iter->to_ )
      continue;

    // everything between [begin_iter, end_iter) has the same To feature pointer
    add_one_flipped_match( inv_set, current_view, begin_iter, end_iter );
    // adjust iterator
    begin_iter = end_iter;
  }

  // the final match
  add_one_flipped_match( inv_set, current_view, begin_iter, end_iter );

  return inv_set;
}
