#include <iostream>
#include <vector>
#include <algorithm>
#include "rrel_quad_est.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_point_2d.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

rrel_quad_est::
rrel_quad_est( const std::vector< vgl_point_2d<double> > & from_pts,
                 const std::vector< vgl_point_2d<double> > & to_pts )
  : rrel_estimation_problem( 12, 6 /*points to instantiate*/ )
{
  assert( from_pts.size() == to_pts.size() );

  constexpr unsigned dim = 2;
  const unsigned size = from_pts.size();

  // convert from vector to vnl_vector type
  vnl_vector<double> pt( dim );
  for ( unsigned int i=0; i<size; ++i ) {
    pt[0] = from_pts[i].x();
    pt[1] = from_pts[i].y();
    from_pts_[i] = pt;
    pt[0] = to_pts[i].x();
    pt[1] = to_pts[i].y();
    to_pts_[i] = pt;
  }

  dim_ = dim;
  quad_dof_ = (dim+1)*dim;
  min_num_pts_ = dim+1;
  num_samples_ = size;
}

rrel_quad_est::
rrel_quad_est( const std::vector< vnl_vector<double> > & from_pts,
                 const std::vector< vnl_vector<double> > & to_pts,
                 unsigned int dim )
: rrel_estimation_problem( ((dim+3)*dim/2+1)*dim /*dof*/,
                           ((dim+3)*dim/2+1)/*points to instantiate*/ ),
    from_pts_( from_pts ), to_pts_( to_pts )
{
  // only deals with 2D for now
  assert( dim == 2 );
  assert( from_pts.size() == to_pts.size() );
  const unsigned size = from_pts.size();

  dim_ = dim;
  quad_dof_ = ((dim+3)*dim/2+1)*dim;
  min_num_pts_ = (dim+3)*dim/2+1;
  num_samples_ = size;
}

rrel_quad_est::~rrel_quad_est() = default;


unsigned int
rrel_quad_est::num_samples( ) const
{
  return num_samples_;
}

inline
void expand_quad( const vnl_vector<double>& s,
                  vnl_vector<double>& des,
                  unsigned ind=0)
{
  // 1.0
  des(ind++) = 1.0;

  // affine term
  for ( unsigned int i=0; i<s.size(); ++i )
    des(ind++) = s(i);

  // cross term
  for ( unsigned int i=0; i<s.size()-1; ++i )
    for ( unsigned int j=i+1; j<s.size(); ++j )
      des(ind++) = s(i) * s(j);

  // quad term
  for ( unsigned int i=0; i<s.size(); ++i )
    des(ind++) = s(i)*s(i);
}

inline
void copy_to_nth_pos( vnl_vector<double>& des, unsigned c,
                      const vnl_vector<double>& s )
{
  for ( unsigned int i=0; i<s.size(); ++i )
    des(c+i) = s(i);
}

vnl_matrix<double>
rrel_quad_est::
A( const vnl_vector<double>& params ) const
{
  vnl_matrix<double> A( dim_, min_num_pts_, 0.0 );
  for ( unsigned ind=0,i=0; i<min_num_pts_; ++i )
    for ( unsigned j=0; j<dim_; ++j )
      A(j, i) = params(ind++);  // filling it column first
  return A;
}

vnl_vector<double>
rrel_quad_est::
trans( const vnl_vector<double>& params ) const
{
  vnl_vector<double> trans( dim_, 0.0 );
  for ( unsigned int i=0; i<dim_; ++i ) {
    trans[i] = params( i );
  }
  return trans;
}

//  The equation to be solved is A p = b, where A is a dof_ x dof_
//  array formed from the independent variables and b is dof_ x 1 and
//  formed from the dependent variables.  If A is not full-rank, false
//  is returned.  Otherwise, params = A^{-1} b and true is returned.
//
bool
rrel_quad_est::
fit_from_minimal_set( const std::vector<int>& point_indices,
                      vnl_vector<double>& params ) const
{
  if ( point_indices.size() != min_num_pts_ ) {
    std::cerr << "rrel_quad_est::fit_from_minimal_sample  The number of point "
             << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }

  vnl_matrix<double> A( min_num_pts_,  min_num_pts_, 1.0 );
  vnl_matrix<double> bs( min_num_pts_, dim_, 0.0 );
  vnl_vector<double> expanded( min_num_pts_ );
  for ( unsigned int i=0; i<min_num_pts_; ++i ) {
    int index = point_indices[i];
    expand_quad( from_pts_[index], expanded );
    A.set_row( i, expanded );

    const vnl_vector<double>& one_to = to_pts_[index];
    for ( unsigned int j=0; j<dim_; ++j )
      bs[i][j] = one_to[j];
  }

  vnl_svd<double> svd( A, 1.0e-8 );
  if ( (unsigned int)svd.rank() < min_num_pts_ ) {
    return false;    // singular fit --- no error message needed
  }
  else {
    params.set_size( quad_dof_ );
    vnl_matrix<double> sol;
    sol = svd.inverse() * bs;
    for ( unsigned int j=0; j<min_num_pts_; ++j )
      copy_to_nth_pos( params, j*dim_, sol.get_row(j) );
    return true;
  }
}


void
rrel_quad_est::compute_residuals( const vnl_vector<double>& params,
                                          std::vector<double>& residuals ) const
{
  assert( residuals.size() == num_samples_ );

  vnl_matrix<double> A( dim_, min_num_pts_ );
  for ( unsigned ind=0,i=0; i<min_num_pts_; ++i )
    for ( unsigned j=0; j<dim_; ++j )
      A(j, i) = params(ind++);  // filling it column first

  vnl_vector<double> diff;
  vnl_vector<double> expanded( min_num_pts_, 0.0 );
  for ( unsigned int i=0; i<num_samples_; ++i ) {
    expand_quad( from_pts_[i], expanded );
    diff = A * expanded;
    diff -= to_pts_[i];
    residuals[i] = diff.two_norm();
  }
}


bool
rrel_quad_est::
weighted_least_squares_fit( vnl_vector<double>& params,
                            vnl_matrix<double>& norm_covar,
                            const std::vector<double>* weights ) const
{
  vnl_matrix<double> sumProds(min_num_pts_, min_num_pts_, 0.0);
  vnl_matrix<double> sumDists(min_num_pts_, dim_, 0.0);

  std::vector<double> tmp_wgts;
  if ( !weights ) {
    // set weight to one
    tmp_wgts.resize( num_samples_ );
    std::fill( tmp_wgts.begin(), tmp_wgts.end(), 1.0 );
    weights = &tmp_wgts;
  }
  //  Aside:  this probably would be faster if I used iterators...

  vnl_vector<double> ind_vars( min_num_pts_, 1.0 );
  for ( unsigned int i=0; i<num_samples_; ++i ) {
    // copy first #dim# elements
    expand_quad( from_pts_[i], ind_vars );
    for ( unsigned int j=0; j<min_num_pts_; ++j ) {
      for ( unsigned int k=j; k<min_num_pts_; k++ )
        sumProds(j,k) += ind_vars[j] * ind_vars[k] * (*weights)[i];

      // set right hand side
      for ( unsigned int k=0; k<dim_; ++k )
        sumDists(j,k) += ind_vars[j] * to_pts_[i][k] * (*weights)[i];
    }
  }

  for ( unsigned int j=1; j<min_num_pts_; j++ )
    for ( unsigned int k=0; k<j; k++ )
      sumProds(j,k) = sumProds(k,j);

  vnl_svd<double> svd( sumProds, 1.0e-8 );
  if ( (unsigned int)svd.rank() < min_num_pts_ ) {
    std::cerr << "rrel_quad_est::WeightedLeastSquaresFit --- singularity!\n";
    return false;
  }
  else {
    vnl_matrix<double> sumP_inv( svd.inverse() );
    vnl_matrix<double> int_result = sumP_inv * sumDists;
    norm_covar = sumP_inv;

    params.set_size( quad_dof_ );
    for ( unsigned int j=0; j<min_num_pts_; ++j )
      copy_to_nth_pos( params, j*dim_, int_result.get_row(j) );
    return true;
  }
}
