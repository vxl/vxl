#include "rrel_affine_est.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>
#include <vgl/vgl_point_2d.h>
#include <vnl/vnl_math.h>

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>

rrel_affine_est::
rrel_affine_est( const vcl_vector< vgl_point_2d<double> > & from_pts,
                 const vcl_vector< vgl_point_2d<double> > & to_pts )
  : rrel_estimation_problem( 6, 3 /*points to instantiate*/ )
{

  assert( from_pts.size() == to_pts.size() );
  
  const unsigned dim  = 2;
  const unsigned size = from_pts.size();

  // convert from vector to vnl_vector type
  vnl_vector<double> pt( dim );
  for( unsigned int i=0; i<size; ++i ) {
    pt[0] = from_pts[i].x();
    pt[1] = from_pts[i].y();
    from_pts_[i] = pt;
    pt[0] = to_pts[i].x();
    pt[1] = to_pts[i].y();
    to_pts_[i] = pt;
  }
  
  affine_dof_ = (dim+1)*dim;
  min_num_pts_ = dim+1;
  num_samples_ = size;
}

rrel_affine_est:: 
rrel_affine_est( const vcl_vector< vnl_vector<double> > & from_pts,
                 const vcl_vector< vnl_vector<double> > & to_pts,
                 unsigned int dim )
  : rrel_estimation_problem( (dim+1)*dim /*dof*/,  dim+1/*points to instantiate*/ ), 
    from_pts_( from_pts ), to_pts_( to_pts )
{
  assert( from_pts.size() == to_pts.size() );
  const unsigned size = from_pts.size(); 

  affine_dof_ = (dim+1)*dim;
  min_num_pts_ = dim+1;
  num_samples_ = size;
}

rrel_affine_est::~rrel_affine_est()
{
}


unsigned int
rrel_affine_est::num_samples( ) const
{
  return num_samples_;
}

inline 
void set_row_wise( vnl_matrix<double>& des, unsigned r, unsigned c, 
                   const vnl_vector<double>& s )
{
  for( unsigned int i=0; i<s.size(); ++i )
    des(r, c+i) = s(i);
 
}

inline 
void copy_to_nth_pos( vnl_vector<double>& des, unsigned c, 
                      const vnl_vector<double> s )
{
  for( unsigned int i=0; i<s.size(); ++i )
    des(c+i) = s(i);
}

vnl_matrix<double> 
rrel_affine_est::
A( const vnl_vector<double>& params ) const
{
  vnl_matrix<double> A( min_num_pts_-1, min_num_pts_-1, 0.0 );
  unsigned index;
  for( unsigned int i=0; i<min_num_pts_-1; ++i ) {
    index = i*min_num_pts_;
    for( unsigned int j=0; j<min_num_pts_-1; ++j ) 
      A(i,j) = params(index+j);
  }
  return A;
}

vnl_vector<double> 
rrel_affine_est::
trans( const vnl_vector<double>& params ) const
{
  vnl_vector<double> trans( min_num_pts_-1, 0.0 );
  const unsigned shift = min_num_pts_-1;
  for( unsigned int i=0; i<min_num_pts_-1; ++i ) {
    trans[i] = params( i*min_num_pts_ + shift );
    
  }
  return trans;
}
  
//  The equation to be solved is A p = b, where A is a dof_ x dof_
//  array formed from the independent variables and b is dof_ x 1 and
//  formed from the dependent variables.  If A is not full-rank, false
//  is returned.  Otherwise, params = A^{-1} b and true is returned.
//
bool
rrel_affine_est::
fit_from_minimal_set( const vcl_vector<int>& point_indices,
                      vnl_vector<double>& params ) const
{
  if ( point_indices.size() != min_num_pts_ ) {
    vcl_cerr << "rrel_affine_est::fit_from_minimal_sample  The number of point "
         << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }

  vnl_matrix<double> A( min_num_pts_,  min_num_pts_, 1.0 );
  vnl_matrix<double> bs( min_num_pts_, min_num_pts_-1, 0.0 );
  for ( unsigned int i=0; i<min_num_pts_; ++i ) {
    int index = point_indices[i];
    set_row_wise( A, i, 0, from_pts_[index] );
    
    const vnl_vector<double>& one_to = to_pts_[index];
    for( unsigned int j=0; j<min_num_pts_-1; ++j ) 
      bs[i][j] = one_to[j];
  }

  vnl_svd<double> svd( A, 1.0e-8 );
  if ( (unsigned int)svd.rank() < min_num_pts_ ) {
    return false;    // singular fit --- no error message needed
  }
  else {
    params.set_size( affine_dof_ );
    vnl_matrix<double> sol;
    sol = svd.inverse() * bs;
    for( unsigned int j=0; j<min_num_pts_-1; ++j ) 
      copy_to_nth_pos( params, j*min_num_pts_, sol.get_column(j) );
    return true;
  }
}


void
rrel_affine_est::compute_residuals( const vnl_vector<double>& params,
                                          vcl_vector<double>& residuals ) const
{
  assert( residuals.size() == num_samples_ );
  
  const vnl_matrix<double> A=this->A( params );
  const vnl_vector<double> t=trans( params );
  
  vnl_vector<double> diff;
  for( unsigned int i=0; i<num_samples_; ++i ) {
    diff = A*from_pts_[i] + t - to_pts_[i];
    residuals[i] = diff.two_norm();
  }
}


bool
rrel_affine_est::
weighted_least_squares_fit( vnl_vector<double>& params,
                            vnl_matrix<double>& norm_covar,
                            const vcl_vector<double>* weights ) const
{

  vnl_matrix<double> sumProds(min_num_pts_, min_num_pts_, 0.0);
  vnl_matrix<double> sumDists(min_num_pts_, min_num_pts_-1, 0.0);

  vcl_vector<double> tmp_wgts;
  if( !weights ) {
    // set weight to one
    tmp_wgts.resize( num_samples_ );
    vcl_fill( tmp_wgts.begin(), tmp_wgts.end(), 1.0 );
    weights = &tmp_wgts;
  }
  //  Aside:  this probably would be faster if I used iterators...

  vnl_vector<double> ind_vars( min_num_pts_ );
  for ( unsigned int i=0; i<num_samples_; ++i ) {
    // copy first #dim# elements
    copy_to_nth_pos( ind_vars, 0, from_pts_[i] );
    for ( unsigned int j=0; j<min_num_pts_; ++j ) {
      for ( unsigned int k=j; k<min_num_pts_; k++ )
        sumProds(j,k) += ind_vars[j] * ind_vars[k] * (*weights)[i];
        
      // set right hand side
      for( unsigned int k=0; k<min_num_pts_-1; ++k )
        sumDists(j,k) += ind_vars[j] * to_pts_[i][k] * (*weights)[i];
    }
  }
  
  for ( unsigned int j=1; j<min_num_pts_; j++ )
    for ( unsigned int k=0; k<j; k++ )
      sumProds(j,k) = sumProds(k,j);

  vnl_svd<double> svd( sumProds, 1.0e-8 );
  if ( (unsigned int)svd.rank() < min_num_pts_ ) {
    vcl_cerr << "rrel_linear_regression::WeightedLeastSquaresFit --- singularity!\n";
    return false;
  }
  else {
    vnl_matrix<double> sumP_inv( svd.inverse() );
    vnl_matrix<double> int_result = sumP_inv * sumDists;
    norm_covar = sumP_inv;

    params.set_size( affine_dof_ );
    for( unsigned int j=0; j<min_num_pts_-1; ++j ) 
      copy_to_nth_pos( params, j*min_num_pts_, int_result.get_column(j) );
    return true;
  }

}
