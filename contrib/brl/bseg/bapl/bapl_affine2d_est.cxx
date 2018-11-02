// This is brl/bseg/bapl/bapl_affine2d_est.cxx
#include <iostream>
#include <cmath>
#include <utility>
#include "bapl_affine2d_est.h"
//:
// \file
#include <bapl/bapl_lowe_keypoint.h>

#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Constructor from a vector of matches (pairs of keypoint smart pointers)
bapl_affine2d_est::bapl_affine2d_est( const std::vector< bapl_keypoint_match > & matches )
  : rrel_estimation_problem( 6 /*dof*/, 3 /*points to instantiate*/ )
{
  vnl_vector< double > p(2), q(2);

  for (const auto & matche : matches) {
    p[0] = matche.first->location_i();
    p[1] = matche.first->location_j();
    from_pts_.push_back( p );

    q[0] = matche.second->location_i();
    q[1] = matche.second->location_j();
    to_pts_.push_back( q );
  }
}

bapl_affine2d_est::bapl_affine2d_est( std::vector< vnl_vector<double> >  from_pts,
                                      std::vector< vnl_vector<double> >  to_pts )
  : rrel_estimation_problem( 6 /*dof*/, 3 /*points to instantiate*/ ),
    from_pts_(std::move( from_pts )), to_pts_(std::move( to_pts ))
{
  assert( from_pts_.size() == to_pts_.size() );
}

bapl_affine2d_est::~bapl_affine2d_est()
= default;


unsigned int
bapl_affine2d_est::num_samples( ) const
{
  return from_pts_.size();
}


bool
bapl_affine2d_est::fit_from_minimal_set( const std::vector<int>& point_indices,
                                               vnl_vector<double>& params ) const
{
  vnl_matrix< double > A(6, 6, 0.0);
  vnl_vector< double > b(6, 0.0);
  assert( point_indices.size() == 3 );

  for ( int i=0; i<3; ++i ) {
    int loc = point_indices[ i ];
    A( 2*i, 0 ) = A( 2*i+1, 2 ) = from_pts_[ loc ][ 0 ];
    A( 2*i, 1 ) = A( 2*i+1, 3 ) = from_pts_[ loc ][ 1 ];
    A( 2*i, 4 ) = A( 2*i+1, 5 ) = 1.0;
    b( 2*i ) = to_pts_[ loc ][ 0 ];
    b( 2*i+1 ) = to_pts_[ loc ][ 1 ];
  }

  vnl_svd<double> svd( A, 1.0e-8 );
  if ( svd.rank() < 6 ) {
    return false;    // singular fit
  }
  else {
    params = vnl_vector<double>( svd.inverse() * b );
    return true;
  }
}

void
bapl_affine2d_est :: compute_residuals( const vnl_vector<double>& params,
                                              std::vector<double>& residuals ) const
{
  vnl_matrix< double > A(2,2);
  vnl_vector< double > t(2);
  int r,c;
  for ( r=0; r<2; ++r )
    for ( c=0; c<2; ++c )
      A( r, c ) = params[ 2*r + c ];
  t(0) = params[4];
  t(1) = params[5];

  vnl_svd< double > svd_A( A );
  if ( svd_A.rank() < 2 )
    std::cerr << "bapl_affine2d_est :: compute_residuals  rank(A) < 2!!";
  vnl_matrix< double > A_inv( svd_A.inverse() );

  if ( residuals.size() != from_pts_.size() )
    residuals.resize( from_pts_.size() );

  vnl_vector< double > trans_pt( 2 ), inv_trans_pt( 2 );
  double del_x, del_y, inv_del_x, inv_del_y;

  for ( unsigned int i=0; i<from_pts_.size(); ++i ) {
    trans_pt = (A * from_pts_[ i ]) + t;
    inv_trans_pt = A_inv * (to_pts_[ i ] - t);


    del_x = trans_pt[ 0 ] - to_pts_[ i ][ 0 ];
    del_y = trans_pt[ 1 ] - to_pts_[ i ][ 1 ];
    inv_del_x = inv_trans_pt[ 0 ] - from_pts_[ i ][ 0 ];
    inv_del_y = inv_trans_pt[ 1 ] - from_pts_[ i ][ 1 ];
    residuals [ i ] = std::sqrt( vnl_math::sqr(del_x)     + vnl_math::sqr(del_y)
                              + vnl_math::sqr(inv_del_x) + vnl_math::sqr(inv_del_y) );
  }
}


bool
bapl_affine2d_est :: weighted_least_squares_fit( vnl_vector<double>& params,
                                                 vnl_matrix<double>& /*norm_covar*/, // FIXME: unused parameter
                                                 const std::vector<double>* weights ) const
{
  const std::vector<double> * w;
  if ( weights )
    w = weights;
  else
    w = new std::vector<double>( from_pts_.size(), 1 );

  std::vector< vnl_vector<double> > norm_from, norm_to;
  vnl_vector< double > center_from(2,0.0), center_to(2,0.0);
  double avg_distance_from, avg_distance_to;

  this -> normalize( from_pts_, *w, norm_from, center_from, avg_distance_from );
  this -> normalize( to_pts_, *w, norm_to, center_to, avg_distance_to );

  vnl_matrix< double > A(2*from_pts_.size(), 6, 0.0);
  vnl_vector< double > b(2*from_pts_.size(), 0.0);
  for ( unsigned i=0; i<from_pts_.size(); ++i ) {
    A( 2*i, 0 ) = A( 2*i+1, 2 ) = (*w)[i] * norm_from[ i ][ 0 ];
    A( 2*i, 1 ) = A( 2*i+1, 3 ) = (*w)[i] * norm_from[ i ][ 1 ];
    A( 2*i, 4 ) = A( 2*i+1, 5 ) = (*w)[i];
    b( 2*i ) = (*w)[i] * norm_to[ i ][ 0 ];
    b( 2*i+1 ) = (*w)[i] * norm_to[ i ][ 1 ];
  }

  vnl_svd<double> svd( A, 1.0e-8 );
  bool result;
  if ( svd.rank() < 6 ) {
    result = false;    // singular fit
  }
  else {
    params = ( svd.inverse() * b );

    // invert normalization
    for (int i=0; i<4; ++i)
      params[i] *= (avg_distance_to / avg_distance_from);

    params[4] = avg_distance_to*params[4] + center_to[0]
                 - params[0]*center_from[0] - params[1]*center_from[1];
    params[5] = avg_distance_to*params[5] + center_to[1]
                 - params[2]*center_from[0] - params[3]*center_from[1];


    result = true;
  }

  if ( !weights )
    delete w;

  return result;
}


void
bapl_affine2d_est :: normalize( const std::vector< vnl_vector<double> >& pts,
                                const std::vector< double >& wgts,
                                std::vector< vnl_vector<double> > & norm_pts,
                                vnl_vector< double > & center,
                                double &avg_distance ) const
{
  norm_pts.resize( pts.size() );

  center.set_size(2);
  center.fill(0.0);

  double sum_wgt = 0;
  unsigned int i;

  for ( i=0; i<pts.size(); ++i ) {
    center[0] += wgts[i] * pts[i][0];
    center[1] += wgts[i] * pts[i][1];
    sum_wgt += wgts[i];
  }
  center /= sum_wgt;

  avg_distance = 0;
  for ( i=0; i<pts.size(); ++i ) {
    avg_distance += wgts[i] * std::sqrt( vnl_math::sqr( pts[i][0] - center[0] ) +
                                        vnl_math::sqr( pts[i][1] - center[1] ) );
  }
  avg_distance /= sum_wgt;

  for ( i=0; i<pts.size(); ++i )
    norm_pts[i] = (pts[i] - center)/avg_distance;
}


void
bapl_affine2d_est :: print_points() const
{
}
