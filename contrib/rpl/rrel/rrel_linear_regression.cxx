#include <iostream>
#include <utility>
#include <vector>
#include "rrel_linear_regression.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

rrel_linear_regression::rrel_linear_regression( const std::vector< vnl_vector<double> >& pts,
                                                bool use_intercept )
  : rand_vars_( pts.size() ),
    ind_vars_( pts.size() )
{
  unsigned int num_pts = pts.size();
  unsigned int vect_size = pts[0].size();
  for ( unsigned int i=0; i<num_pts; ++i ) {
    rand_vars_[i] = pts[i][ vect_size-1 ];
  }

  //
  //  If there is to be an intercept parameter, the first value in each
  //  independent variable vector will be a 1.0 and the remaining values
  //  are taken from locations 0 .. vect_size-2 of each point.
  //
  if ( use_intercept ) {
    set_param_dof( vect_size );
    for ( unsigned int i=0; i<num_pts; ++i ) {
      ind_vars_[i] = vnl_vector<double>(param_dof());
      ind_vars_[i][0] = 1.0;
      for ( unsigned int j=1; j<vect_size; ++j )
        ind_vars_[i][j] = pts[i][j-1];
    }
  }

  //
  //  Otherwise, with no intercept parameter, the independent variable
  //  vectors are formed from locations 0 .. vect_size-2 of each point.
  //
  else {
    set_param_dof( vect_size-1 );
    for ( unsigned int i=0; i<num_pts; ++i ) {
      ind_vars_[i] = vnl_vector<double>(param_dof());
      for ( unsigned int j=0; j<vect_size-1; ++j )
        ind_vars_[i][j] = pts[i][j];
    }
  }
  if ( param_dof() > num_pts ) {
    std::cerr << "\nrrel_linear_regression::rrel_linear_regression  WARNING:  DoF is greater than\n"
             << "the number of data points.  An infinite set of equally valid\n"
             << "solutions exists.\n";
  }
  set_num_samples_for_fit( param_dof() );
}

// ctor that just copies the independent and dependent variables vectors.
rrel_linear_regression::rrel_linear_regression( const std::vector< vnl_vector<double> >& ind_vars,
                                                std::vector< double >  dep_vars )
  : rand_vars_(std::move(dep_vars)), ind_vars_(ind_vars)
{
  set_param_dof( ind_vars_[0].size() );
  if ( param_dof() > ind_vars.size() ) {
    std::cerr << "rrel_linear_regression::rrel_linear_regression  WARNING:  DoF is greater than\n"
             << "the number of data points.  An infinite set of solutions exists.\n";
  }
  set_num_samples_for_fit( param_dof() );
}


rrel_linear_regression::~rrel_linear_regression() = default;

unsigned int
rrel_linear_regression::num_samples( ) const
{
  return rand_vars_.size();
}


//  The equation to be solved is A p = b, where A is a dof_ x dof_
//  array formed from the independent variables and b is dof_ x 1 and
//  formed from the dependent variables.  If A is not full-rank, false
//  is returned.  Otherwise, params = A^{-1} b and true is returned.
//
bool
rrel_linear_regression::fit_from_minimal_set( const std::vector<int>& point_indices,
                                              vnl_vector<double>& params ) const
{
  if ( point_indices.size() != param_dof() ) {
    std::cerr << "rrel_linear_regression::fit_from_minimal_sample  The number of point "
             << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }
  vnl_matrix<double> A(param_dof(), param_dof());
  vnl_vector<double> b(param_dof());
  for ( unsigned int i=0; i<param_dof(); ++i ) {
    int index = point_indices[i];
    for ( unsigned int j=0; j<param_dof(); ++j ) {
      A(j,i) = ind_vars_[index][j];
    }
    b[i] = rand_vars_[index];
  }

  vnl_svd<double> svd( A, 1.0e-8 );
  if ( (unsigned int)svd.rank() < param_dof() ) {
    return false;    // singular fit --- no error message needed
  }
  else {
    params = vnl_vector<double>( b * svd.inverse() );
    return true;
  }
}


void
rrel_linear_regression::compute_residuals( const vnl_vector<double>& params,
                                           std::vector<double>& residuals ) const
{
  assert( residuals.size() == rand_vars_.size() );

  for ( unsigned int i=0; i<rand_vars_.size(); ++i ) {
    residuals[i] = rand_vars_[i] - dot_product( params, ind_vars_[i] );
  }
}


bool
rrel_linear_regression::weighted_least_squares_fit( vnl_vector<double>& params,
                                                    vnl_matrix<double>& norm_covar,
                                                    const std::vector<double>* weights ) const
{
  // If params and cofact are NULL pointers and the fit is successful,
  // this function will allocate a new vector and a new
  // matrix. Otherwise, it assumes that *params is a 1 x param_dof() vector
  // and that cofact is param_dof() x param_dof() matrix.

  assert( !weights || weights->size() == rand_vars_.size() );

  vnl_matrix<double> sumProds(param_dof(), param_dof(), 0.0);
  vnl_matrix<double> sumDists(param_dof(), 1, 0.0);

  //  Aside:  this probably would be faster if I used iterators...

  if (weights) {
    for ( unsigned int i=0; i<rand_vars_.size(); ++i ) {
      for ( unsigned int j=0; j<param_dof(); ++j ) {
        for ( unsigned int k=j; k<param_dof(); k++ )
          sumProds(j,k) += ind_vars_[i][j] * ind_vars_[i][k] * (*weights)[i];
        sumDists(j,0) += ind_vars_[i][j] * rand_vars_[i] * (*weights)[i];
      }
    }
  }
  else {
    for ( unsigned int i=0; i<rand_vars_.size(); ++i ) {
      for ( unsigned int j=0; j<param_dof(); ++j ) {
        for ( unsigned int k=j; k<param_dof(); k++ )
          sumProds(j,k) += ind_vars_[i][j] * ind_vars_[i][k];
        sumDists(j,0) += ind_vars_[i][j] * rand_vars_[i];
      }
    }
  }

  for ( unsigned int j=1; j<param_dof(); j++ )
    for ( unsigned int k=0; k<j; k++ )
      sumProds(j,k) = sumProds(k,j);

  vnl_svd<double> svd( sumProds, 1.0e-8 );
  if ( (unsigned int)svd.rank() < param_dof() ) {
    std::cerr << "rrel_linear_regression::WeightedLeastSquaresFit --- singularity!\n";
    return false;
  }
  else {
    vnl_matrix<double> sumP_inv( svd.inverse() );
    vnl_matrix<double> int_result = sumP_inv * sumDists;
    params = int_result.get_column(0);
    norm_covar = sumP_inv;
    return true;
  }
}


void
rrel_linear_regression::print_points() const
{
  std::cout << "\nrrel_linear_regression::print_points:\n"
           << "  param_dof() = " << param_dof() << '\n'
           << "  num_pts = " << rand_vars_.size() << "\n\n"
           << " i   rand_vars_   ind_vars_\n"
           << " =   ==========   =========\n";
  for ( unsigned int i=0; i<rand_vars_.size(); ++i ) {
    std::cout << ' ' << i << "   " << rand_vars_[i] << "    " << ind_vars_[i] << '\n';
  }
}
