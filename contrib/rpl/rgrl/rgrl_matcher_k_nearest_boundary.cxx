//:
// \file
// \author Charlene Tsai
// \date   Sep 2003

#include "rgrl_matcher_k_nearest_boundary.h"
#include "rgrl_match_set.h"
#include "rgrl_feature_set.h"
#include "rgrl_feature_trace_pt.h"
#include "rgrl_cast.h"
#include <vcl_cassert.h>

rgrl_matcher_k_nearest_boundary::
rgrl_matcher_k_nearest_boundary( unsigned int k )
  : rgrl_matcher_k_nearest(k)
{
}

rgrl_matcher_k_nearest_boundary::
rgrl_matcher_k_nearest_boundary( unsigned int k, double dist_thres  )
  : rgrl_matcher_k_nearest(k, dist_thres)
{
}

rgrl_match_set_sptr
rgrl_matcher_k_nearest_boundary::
compute_matches( rgrl_feature_set const&       from_set,
                 rgrl_feature_set const&       to_set,
                 rgrl_view const&              current_view,
                 rgrl_transformation const&    current_xform,
                 rgrl_scale const&             current_scale ) const
{
  typedef rgrl_match_set::from_iterator FIter;
  typedef FIter::to_iterator TIter;
  typedef vcl_vector<rgrl_feature_sptr >  feature_vector;
  typedef feature_vector::const_iterator fvec_Iter;

  //rgrl_matcher_k_nearest_boundary only works on RGRL_TRACE_PT
  //
  assert(from_set.type() == rgrl_feature_trace_pt::type_id()
         && to_set.type() == rgrl_feature_trace_pt::type_id());

  //1. get the center-point matches using the base class implementation
  //
  rgrl_match_set_sptr cp_matches =
    rgrl_matcher_k_nearest::compute_matches(from_set, to_set, current_view,
                                            current_xform, current_scale);
  rgrl_match_set_sptr bd_matches = new rgrl_match_set( from_set.type(), to_set.type() );
  if ( cp_matches->from_size() == 0 ) return bd_matches;

  int m = cp_matches->from_begin().from_feature()->location().size();
  assert( m == 2 ); //For now, only deal with 2D

  //2. For each center-point match, compute the bounary matches
  //
  for ( FIter fi = cp_matches->from_begin(); fi != cp_matches->from_end(); ++fi ) {
    rgrl_feature_trace_pt* from_feature =
      rgrl_cast<rgrl_feature_trace_pt *>(fi.from_feature());
    feature_vector from_bd_pts = from_feature->boundary_points(vnl_vector<double>());

    //transform the boundary points
    feature_vector mapped_bd_pts;
    for (fvec_Iter mi = from_bd_pts.begin(); mi != from_bd_pts.end(); ++mi) {
      mapped_bd_pts.push_back((*mi)->transform( current_xform ));
    }

    //assign the correponding to_pt_boundary points to the from_boundary_points
    vcl_vector<feature_vector> to_bd_pt_set(from_bd_pts.size());
    for ( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
      rgrl_feature_trace_pt* to_feature =
        rgrl_cast<rgrl_feature_trace_pt *>(ti.to_feature());
      feature_vector to_bd_pts = to_feature->boundary_points(vnl_vector<double>());
      vbl_array_2d<bool> assignment = match_boundary_pts(mapped_bd_pts, to_bd_pts);
      for (int i = 0; i<assignment.rows(); ++i)
        for (int j = 0; j<assignment.cols(); ++j){
          if (assignment(i,j)) to_bd_pt_set[i].push_back(to_bd_pts[j]);
        }
    }

    // Store the results into the bd_matches
    for (unsigned i = 0; i<from_bd_pts.size(); ++i) {
      bd_matches->add_feature_and_matches( from_bd_pts[i], mapped_bd_pts[i],
                                           to_bd_pt_set[i]);
    }
  }

  return bd_matches;
}


vbl_array_2d<bool>
rgrl_matcher_k_nearest_boundary::
match_boundary_pts(feature_vector const& from_pts,
                   feature_vector const& to_pts) const
{
  assert(from_pts.size() == to_pts.size());

  vbl_array_2d<double> dist_errors( from_pts.size(), to_pts.size());
  for (int i = 0; i<dist_errors.rows(); ++i) {
    for (int j = 0; j<dist_errors.cols(); ++j) {
      dist_errors(i,j) = vnl_vector_ssd(from_pts[i]->location(),to_pts[j]->location());
    }
  }

  vbl_array_2d<bool> assignment(from_pts.size(), to_pts.size(), false);

  if ( from_pts.size() == 2 ) {
    if ( dist_errors(0,0) + dist_errors(1,1) >  dist_errors(0,1) + dist_errors(1,0) )
      assignment(0,1) = assignment(1,0) = true;
    else assignment(0,0) = assignment(1,1) = true;
  }
  else { //for more than 2 boundary points, using dynamic programming
    vbl_array_2d<bool> valid(from_pts.size(), to_pts.size(), true);
    double min_obj;
    vbl_array_2d<bool> assignment = match_boundary_pts_helper(dist_errors, valid, from_pts.size(), min_obj);
  }

  return assignment;
}

vbl_array_2d<bool>
rgrl_matcher_k_nearest_boundary::
match_boundary_pts_helper(vbl_array_2d<double> const& dist_error,
                          vbl_array_2d<bool> const& valid,
                          int count, double& obj_value) const
{
  if (count == 0) {
    obj_value = 0.0;
    return vbl_array_2d<bool>(dist_error.rows(), dist_error.cols(), false);
  }

  vbl_array_2d<bool> best_assignment;
  double obj;
  bool min_obj_set = false;

  for (int i = 0; i < valid.cols(); i++) {
    if (valid(count-1,i)) {
      vbl_array_2d<bool> valid2(valid);
      for ( int j = 0; j < valid.rows(); j++)
         valid2.put(j,i,false);
      for ( int j = 0; j < valid.cols(); j++)
         valid2.put(count-1,j,false);
      vbl_array_2d<bool> assignment =
        match_boundary_pts_helper(dist_error, valid2, count-1, obj);
      obj += dist_error(count-1,i);
      if (!min_obj_set || obj_value > obj) {
        min_obj_set = true;
        obj_value = obj;
        best_assignment =  assignment;
        best_assignment(count-1,i) = true;
      }
    }
  }

  return best_assignment;
}

