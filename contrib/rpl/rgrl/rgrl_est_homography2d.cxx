#include "rgrl_est_homography2d.h"
#include "rgrl_trans_homography2d.h"
#include "rgrl_match_set.h"

#include <vnl/algo/vnl_svd.h>
#include <vnl/vnl_math.h>

rgrl_est_homography2d::
rgrl_est_homography2d( double condition_num_thrd )
  : condition_num_thrd_( condition_num_thrd )
{
   rgrl_estimator::set_param_dof( 8 );
}

rgrl_transformation_sptr 
rgrl_est_homography2d::
estimate( rgrl_set_of<rgrl_match_set_sptr> const& matches,
          rgrl_transformation const& /*cur_transform*/ ) const
{
  // Normalization of the input data by s similarity transformation,
  // such that the new center is the origin, and the the average
  // distance from the origin is sqrt(2).
  //
  vcl_vector< vnl_vector<double> > norm_from_pts, norm_to_pts;
  vcl_vector< double > wgts;
  double from_scale, to_scale;
  vnl_vector<double> from_center(2), to_center(2);
  if( !normalize( matches, norm_from_pts, norm_to_pts, wgts, from_scale, to_scale, 
                  from_center, to_center ) ) {
    return 0;
  }
                  

  vnl_matrix< double > A( 2*norm_from_pts.size(), 9, 0.0 );
  for ( unsigned int i=0; i<norm_from_pts.size(); ++i ) {
    A( 2*i, 0 ) = A( 2*i+1, 3 ) = wgts[i] * norm_from_pts[i][0] * norm_to_pts[i][2];
    A( 2*i, 1 ) = A( 2*i+1, 4 ) = wgts[i] * norm_from_pts[i][1] * norm_to_pts[i][2];
    A( 2*i, 2 ) = A( 2*i+1, 5 ) = wgts[i] * norm_from_pts[i][2] * norm_to_pts[i][2];
    A( 2*i, 6 ) = wgts[i] * -1 * norm_from_pts[i][0] * norm_to_pts[i][0];
    A( 2*i, 7 ) = wgts[i] * -1 * norm_from_pts[i][1] * norm_to_pts[i][0];
    A( 2*i, 8 ) = wgts[i] * -1 * norm_from_pts[i][2] * norm_to_pts[i][0];
    A( 2*i+1, 6 ) = wgts[i] * -1 * norm_from_pts[i][0] * norm_to_pts[i][1];
    A( 2*i+1, 7 ) = wgts[i] * -1 * norm_from_pts[i][1] * norm_to_pts[i][1];
    A( 2*i+1, 8 ) = wgts[i] * -1 * norm_from_pts[i][2] * norm_to_pts[i][1];
  }
  vnl_svd<double> svd( A, 1.0e-8 );

  if ( svd.rank() < 8 ) {
    DebugMacro(1, "rank ("<<svd.rank()<<")  no solution." );
    return 0; // no solution
  }
  else {
    vnl_vector< double > nparams = svd.nullvector();
    vnl_matrix< double > normH( 3, 3 );
    for ( int r=0; r<3; ++r )
      for ( int c=0; c<3; ++c )
        normH( r, c ) = nparams( 3*r + c );
    
    vnl_matrix<double> to_scale_matrix_inv(3,3,vnl_matrix_identity);
    vnl_matrix<double> from_scale_matrix(3,3,vnl_matrix_identity);
    to_scale_matrix_inv(0,0) = 1/to_scale;
    to_scale_matrix_inv(1,1) = 1/to_scale;
    from_scale_matrix(0,0) = from_scale;
    from_scale_matrix(1,1) = from_scale;
    
    // Denormalization by the scales only. Uncentering will be done
    // during the construction of the transformation.
    //
    vnl_matrix< double > H = to_scale_matrix_inv * normH * from_scale_matrix;
    vnl_matrix<double> covar(9,9,0.0);
    estimate_covariance( norm_from_pts, norm_to_pts, wgts, 
                         from_scale, to_scale, covar);
    
    return new rgrl_trans_homography2d( H, covar, from_center, to_center );
  }
}


rgrl_transformation_sptr
rgrl_est_homography2d::
estimate( rgrl_match_set_sptr matches,
          rgrl_transformation const& cur_transform ) const
{
  // use base class implementation
  return rgrl_estimator::estimate( matches, cur_transform );
}

const vcl_type_info&
rgrl_est_homography2d::
transformation_type() const
{
  return rgrl_trans_homography2d::type_id();
}

bool 
rgrl_est_homography2d::
normalize( rgrl_set_of<rgrl_match_set_sptr> const& matches,
           vcl_vector< vnl_vector<double> >& norm_froms,
           vcl_vector< vnl_vector<double> >& norm_tos,
           vcl_vector< double >& wgts,
           double& from_scale,
           double& to_scale,
           vnl_vector< double > & from_center,
           vnl_vector< double > & to_center ) const
{
  // Iterators to go over the matches
  //
  typedef rgrl_match_set::const_from_iterator FIter;
  typedef FIter::to_iterator TIter;

  vnl_matrix<double> from_norm_matrix(3,3,0.0);
  vnl_matrix<double> to_norm_matrix(3,3,0.0);
  norm_froms.reserve( matches[0]->from_size() );

  // Compute the centers
  //
  from_center.fill( 0.0 );
  to_center.fill( 0.0 );
  vnl_vector<double> from_pt( 2 );
  vnl_vector<double> to_pt( 2 );
  double sum_wgt = 0;

  for( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ){
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        double const wgt = ti.cumulative_weight();
        from_pt = fi.from_feature()->location();
        from_center += from_pt * wgt;
        to_pt = ti.to_feature()->location();
        to_center += to_pt * wgt;
        sum_wgt += wgt;
      }
    }
  }
  // if the weight is too small or zero,
  // that means there is no good match
  if( sum_wgt < 1e-13 ) {
    return false;
  }
  
  from_center /= sum_wgt;
  to_center /= sum_wgt;

  // Compute the average distance
  // 
  double from_avg_distance = 0;
  double to_avg_distance = 0;
  for( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ){
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        double const wgt = ti.cumulative_weight();
        from_pt = fi.from_feature()->location();
        from_avg_distance += 
          wgt * vcl_sqrt( vnl_math_sqr( from_pt[0] - from_center[0] ) +
                          vnl_math_sqr( from_pt[1] - from_center[1] ) );
        to_pt = ti.to_feature()->location();
        to_avg_distance += 
          wgt * vcl_sqrt( vnl_math_sqr( to_pt[0] - to_center[0] ) +
                          vnl_math_sqr( to_pt[1] - to_center[1] ) );
      }
    }
  }
  from_avg_distance /= sum_wgt;
  to_avg_distance /= sum_wgt;
  
  // Put together the similarity transformation for normalization of
  // "from" points
  from_norm_matrix( 0, 0 ) = 1.0 / from_avg_distance;
  from_norm_matrix( 0, 2 ) = -from_center[0] / from_avg_distance;
  from_norm_matrix( 1, 1 ) = 1.0 / from_avg_distance;
  from_norm_matrix( 1, 2 ) = -from_center[1] / from_avg_distance;
  from_norm_matrix( 2, 2 ) = 1.0;
  from_scale = 1.0 / from_avg_distance;
  
  // Put together the similarity transformation for normalization of
  // "to" points
  to_norm_matrix( 0, 0 ) = 1.0 / to_avg_distance;
  to_norm_matrix( 0, 2 ) = -to_center[0] / to_avg_distance;
  to_norm_matrix( 1, 1 ) = 1.0 / to_avg_distance;
  to_norm_matrix( 1, 2 ) = -to_center[1] / to_avg_distance;
  to_norm_matrix( 2, 2 ) = 1.0;
  to_scale = 1.0 / to_avg_distance;

  // Now do the normalization
  vnl_vector<double> norm_from_pt(3,1.0);
  vnl_vector<double> norm_to_pt(3,1.0);
  for( unsigned ms=0; ms < matches.size(); ++ms ) {
    rgrl_match_set const& match_set = *matches[ms];
    for( FIter fi = match_set.from_begin(); fi != match_set.from_end(); ++fi ){
      norm_from_pt[0] = fi.from_feature()->location()[0];
      norm_from_pt[1] = fi.from_feature()->location()[1];
      for( TIter ti = fi.begin(); ti != fi.end(); ++ti ) {
        norm_to_pt[0] = ti.to_feature()->location()[0];
        norm_to_pt[1] = ti.to_feature()->location()[1];
        norm_froms.push_back( from_norm_matrix * norm_from_pt );
        norm_tos.push_back( to_norm_matrix * norm_to_pt );
        wgts.push_back( ti.cumulative_weight() );
      }
    }
  }
  
  return true;
}

void 
rgrl_est_homography2d::
estimate_covariance( const vcl_vector< vnl_vector<double> >& norm_froms,
                     const vcl_vector< vnl_vector<double> >& norm_tos,
                     const vcl_vector< double >& wgts,
                     double from_scale,
                     double to_scale,
                     vnl_matrix<double>& covar ) const
{
  vnl_matrix<double> JTJ(9,9, 0.0);
  vnl_matrix<double> J(2,9, 0.0);
  
  for (unsigned int i = 0; i<norm_froms.size(); i++) {
    J(0,0) = J(1,3) = norm_froms[i][0];
    J(0,1) = J(1,4) = norm_froms[i][1];
    J(0,2) = J(1,5) = norm_froms[i][2];
    
    J(0,6) = -norm_tos[i][0]*norm_froms[i][0];
    J(0,7) = -norm_tos[i][0]*norm_froms[i][1];
    J(0,8) = -norm_tos[i][0]*norm_froms[i][2];

    J(1,6) = -norm_tos[i][1]*norm_froms[i][0];
    J(1,7) = -norm_tos[i][1]*norm_froms[i][1];
    J(1,8) = -norm_tos[i][1]*norm_froms[i][2];
    
    JTJ += wgts[i]*J.transpose()*J;
  }

  vnl_svd<double> svd( J, 1.0e-8 );
  covar = svd.inverse();

  //Please note, the effect of the normalization on the point set is
  //not removed. This *might* be a problem.
}
