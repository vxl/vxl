#include "rrel_shift2d_est.h"

#include <vgl/vgl_homg_point_2d.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_math.h>

#include <vcl_cassert.h>
#include <vcl_cmath.h>

rrel_shift2d_est::rrel_shift2d_est(
    const vcl_vector< vgl_homg_point_2d<double> > & from_pts,
    const vcl_vector< vgl_homg_point_2d<double> > & to_pts )
    : rrel_estimation_problem( 2 /*dof*/, 1 /*points to instantiate*/ )
{
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
  compute_del_pts ();
}

rrel_shift2d_est::rrel_shift2d_est(
    const vcl_vector< vnl_vector<double> > & from_pts,
    const vcl_vector< vnl_vector<double> > & to_pts )
    : rrel_estimation_problem( 2 /*dof*/, 1 /*points to instantiate*/ ),
      from_pts_( from_pts ), to_pts_( to_pts )
{
  assert( from_pts_.size() == to_pts_.size() );
  for ( unsigned int i=0; i<from_pts_.size(); ++i ) {
    assert( from_pts_[ i ][ 2 ] != 0 );
    assert( to_pts_[ i ][ 2 ] != 0 );
  }
  compute_del_pts ();
}

rrel_shift2d_est::~rrel_shift2d_est()
{
}

unsigned int
rrel_shift2d_est::num_samples( ) const
{
  return from_pts_.size();
}

bool
rrel_shift2d_est::fit_from_minimal_set(
    const vcl_vector<int>& point_indices,
    vnl_vector<double>& params ) const
{
  assert( point_indices.size() == 1 );
  int loc = point_indices[ 0 ];
  params = del_pts_[loc];
  return true;
}

void
rrel_shift2d_est::compute_residuals(
    const vnl_vector<double>& params,
    vcl_vector<double>& residuals ) const
{
  assert (2 == params.size() );
  if ( residuals.size() != from_pts_.size() )
      residuals.resize( from_pts_.size() );

  for (unsigned i=0; i<from_pts_.size(); i++) {
      double del_x = del_pts_[i][0] - params[0];
      double del_y = del_pts_[i][1] - params[1];
      residuals[i] = vcl_sqrt( vnl_math_sqr(del_x) + vnl_math_sqr(del_y) );
  }
}

bool
rrel_shift2d_est::weighted_least_squares_fit(
    vnl_vector<double>& params,
    vnl_matrix<double>& /* norm_covar */,
    const vcl_vector<double>* weights ) const
{
  const vcl_vector<double> * w;
  if ( weights )
    w = weights;
  else
    w = new vcl_vector<double>( from_pts_.size(), 1 );

  params.set_size(2);
  double x_del_tot = 0.0;
  double y_del_tot = 0.0;
  double weight_tot = 0.0;

  for (unsigned i=0; i<from_pts_.size(); i++) {
      x_del_tot += del_pts_[i][0] * (*w)[i];
      y_del_tot += del_pts_[i][1] * (*w)[i];
      weight_tot += (*w)[i];
  }

  params[0] = x_del_tot / weight_tot;
  params[1] = y_del_tot / weight_tot;

  if ( !weights )
    delete w;

  return true;
}

void
rrel_shift2d_est::compute_del_pts()
{
  assert (0 == del_pts_.size());

  for (unsigned i=0; i<from_pts_.size(); i++) {
      assert (0.0 != from_pts_[i][2]);
      assert (0.0 != to_pts_[i][2]);
      vnl_vector< double > del (2);
      del[0] = to_pts_[i][0] / to_pts_[i][2]
             - from_pts_[i][0] / from_pts_[i][2];
      del[1] = to_pts_[i][1] / to_pts_[i][2]
             - from_pts_[i][1] / from_pts_[i][2];
      del_pts_.push_back (del);
  }
}
