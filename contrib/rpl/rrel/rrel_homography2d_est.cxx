// This is rpl/rrel/rrel_homography2d_est.cxx
#include "rrel_homography2d_est.h"

#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>
#include <vnl/algo/vnl_svd.h>

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_cmath.h>

rrel_homography2d_est :: rrel_homography2d_est( const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
                                                const vcl_vector< vgl_homg_point_2d<double> > & to_pts,
                                                const int homog_dof )
  : rrel_estimation_problem( homog_dof /*dof*/, ( homog_dof / 2 ) /*points to instantiate*/ )
{
  assert( homog_dof == ( homog_dof / 2 ) * 2 ); // Make sure DOF is even
  assert( from_pts.size() == to_pts.size() );
  vnl_vector< double > p(3), q(3);

  for ( unsigned int i=0; i<from_pts.size(); ++i ) {
    p[0] = from_pts[i].x();
    p[1] = from_pts[i].y();
    p[2] = from_pts[i].w();
    assert( p[2] != 0 );
    from_pts_.push_back( p );
    q[0] = to_pts[i].x();
    q[1] = to_pts[i].y();
    q[2] = to_pts[i].w();
    assert( q[2] != 0 );
    to_pts_.push_back( q );
  }

  homog_dof_ = homog_dof;
  min_num_pts_ = homog_dof_ / 2;
}

rrel_homography2d_est :: rrel_homography2d_est( const vcl_vector< vnl_vector<double> > & from_pts,
                                                const vcl_vector< vnl_vector<double> > & to_pts,
                                                const int homog_dof )
  : rrel_estimation_problem( homog_dof /*dof*/, ( homog_dof / 2 ) /*points to instantiate*/ ),
    from_pts_( from_pts ), to_pts_( to_pts )
{
  assert( homog_dof == ( homog_dof / 2 ) * 2 ); // Make sure DOF is even
  assert( from_pts_.size() == to_pts_.size() );
  for ( unsigned int i=0; i<from_pts_.size(); ++i ) {
    assert( from_pts_[ i ][ 2 ] != 0 );
    assert( to_pts_[ i ][ 2 ] != 0 );
  }

  homog_dof_ = homog_dof;
  min_num_pts_ = homog_dof_ / 2;
}

rrel_homography2d_est::~rrel_homography2d_est()
{
}


unsigned int
rrel_homography2d_est :: num_samples( ) const
{
  return from_pts_.size();
}


bool
rrel_homography2d_est :: fit_from_minimal_set( const vcl_vector<int>& point_indices,
                                               vnl_vector<double>& params ) const
{
  vnl_matrix< double > A(9, 9, 0.0);
  assert( point_indices.size() == min_num_pts_ );

  for ( int i=0; i<min_num_pts_; ++i ) {
    int loc = point_indices[ i ];
    A( 2*i, 0 ) = A( 2*i+1, 3 ) = from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 1 ) = A( 2*i+1, 4 ) = from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 2 ) = A( 2*i+1, 5 ) = from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 2 ];
    A( 2*i, 6 ) = -1 * from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 0 ];
    A( 2*i, 7 ) = -1 * from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 0 ];
    A( 2*i, 8 ) = -1 * from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 0 ];
    A( 2*i+1, 6 ) = -1 * from_pts_[ loc ][ 0 ] * to_pts_[ loc ][ 1 ];
    A( 2*i+1, 7 ) = -1 * from_pts_[ loc ][ 1 ] * to_pts_[ loc ][ 1 ];
    A( 2*i+1, 8 ) = -1 * from_pts_[ loc ][ 2 ] * to_pts_[ loc ][ 1 ];
  }

  vnl_svd<double> svd( A, 1.0e-8 );

  if ( svd.rank() < homog_dof_ ) {
    return false;    // singular fit
  }
  else {
    params = svd.nullvector();
    return true;
  }
}

void
rrel_homography2d_est :: compute_residuals( const vnl_vector<double>& params,
                                            vcl_vector<double>& residuals ) const
{
  vnl_matrix< double > H(3,3);
  int r,c;
  for ( r=0; r<3; ++r )
    for ( c=0; c<3; ++c )
      H( r, c ) = params[ 3*r + c ];

  vnl_svd< double > svd_H( H );
  if ( svd_H.rank() < 3 )
    vcl_cerr << "rrel_homography2d_est :: compute_residuals  rank(H) < 3!!";
  vnl_matrix< double > H_inv( svd_H.inverse() );

  if ( residuals.size() != from_pts_.size() )
    residuals.resize( from_pts_.size() );

  vnl_vector< double > trans_pt( 3 ), inv_trans_pt( 3 );
  double del_x, del_y, inv_del_x, inv_del_y;

  for ( unsigned int i=0; i<from_pts_.size(); ++i ) {
    trans_pt = H * from_pts_[ i ];
    inv_trans_pt = H_inv * to_pts_[ i ];

    if ( from_pts_[ i ][ 2 ] == 0 || to_pts_[ i ][ 2 ] == 0
         || trans_pt[ 2 ] == 0 || inv_trans_pt[ 2 ] == 0 ) {
      residuals[ i ] = 1e10;
    }
    else {
      del_x = trans_pt[ 0 ] / trans_pt[ 2 ] - to_pts_[ i ][ 0 ] / to_pts_[ i ][ 2 ];
      del_y = trans_pt[ 1 ] / trans_pt[ 2 ] - to_pts_[ i ][ 1 ] / to_pts_[ i ][ 2 ];
      inv_del_x = inv_trans_pt[ 0 ] / inv_trans_pt[ 2 ] - from_pts_[ i ][ 0 ] / from_pts_[ i ][ 2 ];
      inv_del_y = inv_trans_pt[ 1 ] / inv_trans_pt[ 2 ] - from_pts_[ i ][ 1 ] / from_pts_[ i ][ 2 ];
      residuals [ i ] = vcl_sqrt( vnl_math_sqr(del_x)     + vnl_math_sqr(del_y)
                                + vnl_math_sqr(inv_del_x) + vnl_math_sqr(inv_del_y) );
    }
  }
}


bool
rrel_homography2d_est :: weighted_least_squares_fit( vnl_vector<double>& params,
                                                     vnl_matrix<double>& norm_covar,
                                                     const vcl_vector<double>* weights ) const
{
  const vcl_vector<double> * w;
  if ( weights )
    w = weights;
  else
    w = new vcl_vector<double>( from_pts_.size(), 1 );

  vcl_vector< vnl_vector<double> > norm_from, norm_to;
  vnl_matrix< double > norm_matrix_from(3,3), norm_matrix_to(3,3);

  this -> normalize( from_pts_, *w, norm_from, norm_matrix_from );
  this -> normalize( to_pts_, *w, norm_to, norm_matrix_to );

  vnl_matrix< double > A( 2*from_pts_.size(), 9, 0.0 );
  for ( unsigned int i=0; i<from_pts_.size(); ++i ) {
    A( 2*i, 0 ) = A( 2*i+1, 3 ) = (*w)[i] * norm_from[ i ][ 0 ] * norm_to[ i ][ 2 ];
    A( 2*i, 1 ) = A( 2*i+1, 4 ) = (*w)[i] * norm_from[ i ][ 1 ] * norm_to[ i ][ 2 ];
    A( 2*i, 2 ) = A( 2*i+1, 5 ) = (*w)[i] * norm_from[ i ][ 2 ] * norm_to[ i ][ 2 ];
    A( 2*i, 6 ) = (*w)[i] * -1 * norm_from[ i ][ 0 ] * norm_to[ i ][ 0 ];
    A( 2*i, 7 ) = (*w)[i] * -1 * norm_from[ i ][ 1 ] * norm_to[ i ][ 0 ];
    A( 2*i, 8 ) = (*w)[i] * -1 * norm_from[ i ][ 2 ] * norm_to[ i ][ 0 ];
    A( 2*i+1, 6 ) = (*w)[i] * -1 * norm_from[ i ][ 0 ] * norm_to[ i ][ 1 ];
    A( 2*i+1, 7 ) = (*w)[i] * -1 * norm_from[ i ][ 1 ] * norm_to[ i ][ 1 ];
    A( 2*i+1, 8 ) = (*w)[i] * -1 * norm_from[ i ][ 2 ] * norm_to[ i ][ 1 ];
  }

  vnl_svd<double> svd( A, 1.0e-8 );

  bool result;
  if ( svd.rank() < homog_dof_ ) {
    result= false;
  }
  else {
    vnl_vector< double > nparams = svd.nullvector();
    vnl_matrix< double > normH( 3, 3 );
    params_to_homog(nparams, normH);

    vnl_svd<double> svd_norm_to( norm_matrix_to );
    assert( svd_norm_to.rank() == 3 );
    vnl_matrix< double > H = svd_norm_to.inverse() * normH * norm_matrix_from;

    params.set_size(9);
    homog_to_params(H, params);

    result = true;
  }

  if ( !weights )
    delete w;

  return result;
}

void
rrel_homography2d_est :: homog_to_params(const vnl_matrix<double>& m,
                                         vnl_vector<double>&       p) const
{
    for ( int r=0; r<3; ++r )
      for ( int c=0; c<3; ++c )
        p( 3*r + c ) = m( r, c );
}

void
rrel_homography2d_est :: params_to_homog(const vnl_vector<double>& p,
                                         vnl_matrix<double>&       m) const
{
    for ( int r=0; r<3; ++r )
      for ( int c=0; c<3; ++c )
        m( r, c ) = p( 3*r + c );
}

void
rrel_homography2d_est :: normalize( const vcl_vector< vnl_vector<double> >& pts,
                                    const vcl_vector< double >& wgts,
                                    vcl_vector< vnl_vector<double> > & norm_pts,
                                    vnl_matrix< double > & norm_matrix ) const
{
  norm_pts.resize( pts.size() );
  norm_matrix.set_size( 3, 3 );
  norm_matrix.fill( 0.0 );

  vnl_vector<double> center( 2, 0.0 );
  double sum_wgt = 0;
  unsigned int i;

  for ( i=0; i<pts.size(); ++i ) {
    center.x() += wgts[i] * pts[i][0] / pts[i][2];
    center.y() += wgts[i] * pts[i][1] / pts[i][2];
    sum_wgt += wgts[i];
  }
  center /= sum_wgt;

  double avg_distance = 0;
  for ( i=0; i<pts.size(); ++i ) {
    avg_distance += wgts[i] * vcl_sqrt( vnl_math_sqr( pts[i][0] / pts[i][2] - center.x() ) +
                                        vnl_math_sqr( pts[i][1] / pts[i][2] - center.y() ) );
  }
  avg_distance /= sum_wgt;

  norm_matrix( 0, 0 ) = 1.0 / avg_distance;
  norm_matrix( 0, 2 ) = -center.x() / avg_distance;
  norm_matrix( 1, 1 ) = 1.0 / avg_distance;
  norm_matrix( 1, 2 ) = -center.y() / avg_distance;
  norm_matrix( 2, 2 ) = 1.0;

  for ( i=0; i<pts.size(); ++i )
    norm_pts[i] = norm_matrix * pts[i];
}


void
rrel_homography2d_est :: print_points() const
{
}
