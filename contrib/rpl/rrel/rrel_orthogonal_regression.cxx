#include <iostream>
#include <vector>
#include "rrel_orthogonal_regression.h"

#include <vnl/vnl_matrix.h>
#include <vnl/vnl_math.h>
#include <vnl/vnl_vector.h>
#include <vnl/algo/vnl_svd.h>

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

rrel_orthogonal_regression::rrel_orthogonal_regression( const vnl_matrix<double>& pts )
  : vars_( pts )
{
  unsigned int num_pts = pts.rows ();
  set_param_dof( pts.cols() );
  if ( param_dof() > num_pts )
    std::cerr << "\nrrel_orthogonal_regression::rrel_orthogonal_regression  WARNING:\n"
             << "DoF ("<<param_dof()<<") is greater than the number of data points ("
             << num_pts << ").\nAn infinite set of equally valid solutions exists.\n";
  set_num_samples_for_fit( param_dof() );
}


rrel_orthogonal_regression::rrel_orthogonal_regression( const std::vector<vnl_vector<double> >& pts )
  : vars_( pts.size(),pts[0].size() )
{
  unsigned int num_pts = vars_.rows();

  for (unsigned int i=0;i<num_pts;i++)
    vars_.set_row(i, pts[i]);

  set_param_dof( vars_.cols() ); // up to a scale
  if ( param_dof() > num_pts )
    std::cerr << "\nrrel_orthogonal_regression::rrel_orthogonal_regression  WARNING:\n"
             << "DoF ("<<param_dof()<<") is greater than the number of data points ("
             << num_pts << ").\nAn infinite set of equally valid solutions exists.\n";
  set_num_samples_for_fit( param_dof() );
}


rrel_orthogonal_regression::~rrel_orthogonal_regression() = default;

unsigned int
rrel_orthogonal_regression::num_samples( ) const
{
  return vars_.rows();
}

bool
rrel_orthogonal_regression::fit_from_minimal_set( const std::vector<int>& point_indices,
                                                  vnl_vector<double>& params ) const
{
  if ( point_indices.size() != param_dof() ) {
    std::cerr << "rrel_orthogonal_regression::fit_from_minimal_sample  The number of point "
             << "indices must agree with the fit degrees of freedom.\n";
    return false;
  }

  // The equation to be solved is Ap = 0, where A is a dof_ x (dof_+1)
  // because the solution is up to a scale.
  vnl_matrix<double> A(param_dof(), param_dof()+1, 1);
  for (unsigned int i=0; i<param_dof(); ++i) {
    int index = point_indices[i];
    for ( unsigned int j=0; j<param_dof(); ++j) {
      A(i,j) = vars_(index, j);
    }
  }
  vnl_svd<double> svd( A, 1.0e-8 );
  if ( (unsigned int)svd.rank() < param_dof() ) {
    std::cerr << "rrel_orthogonal_regression:: singular fit!\n";
    return false;    // singular fit
  }
  else {
    params = svd.nullvector();
    params /= std::sqrt( 1 - vnl_math::sqr( params[ params.size()-1 ] ) );
  }
  return true;
}


void
rrel_orthogonal_regression::compute_residuals( const vnl_vector<double>& params,
                                               std::vector<double>& residuals ) const
{
  // The residual is the algebraic distance, which is simply A * p.
  // Assumes the parameter vector has a unit normal.
  assert( residuals.size() == vars_.rows() );

  vnl_vector<double> norm(params.size()-1);
  for (unsigned int i=0; i<params.size()-1; i++)
    norm[i] = params[i];
  assert(norm.two_norm() > 0.99 && norm.two_norm() < 1.01);
  for ( unsigned int i=0; i<vars_.rows(); ++i ) {
    residuals[i] = dot_product(norm, vars_.get_row(i) ) + params[params.size()-1];
  }
}


// Compute a least-squares fit, using the weights if they are provided.
// The cofact matrix is not used or set.
bool
rrel_orthogonal_regression::weighted_least_squares_fit( vnl_vector<double>& params,
                                                        vnl_matrix<double>& /*cofact*/,
                                                        const std::vector<double> *weights ) const
{
  // If params and cofact are NULL pointers and the fit is successful,
  // this function will allocate a new vector and a new
  // matrix. Otherwise, it assumes that *params is a param_dof() x 1 vector
  // and that cofact is param_dof() x param_dof() matrix.

  assert( !weights || weights->size() == vars_.rows() );

  vnl_vector<double> sum_vect(vars_.cols(), 0.0);
  vnl_matrix<double> A(vars_.cols(), vars_.cols());
  vnl_vector<double> avg;
  vnl_matrix<double> shift_vars( vars_.rows(), vars_.cols() );

  if (weights) {
    double sum_weight=0;
    for (unsigned int i=0; i<vars_.rows(); ++i) {
      sum_vect += vars_.get_row(i) * (*weights)[i];
      sum_weight += (*weights)[i];
    }
    avg = sum_vect / sum_weight;
    for (unsigned int i=0; i<vars_.rows(); ++i)
      shift_vars.set_row(i, (vars_.get_row(i)-avg) * std::sqrt((*weights)[i]));
  }
  else {
    for (unsigned int i=0; i<vars_.rows(); ++i)
      sum_vect += vars_.get_row(i);
    avg = sum_vect / vars_.rows();
    for (unsigned int i=0; i<vars_.rows(); ++i)
      shift_vars.set_row(i, vars_.get_row(i) -avg);
  }

  A = shift_vars.transpose() * shift_vars;

  vnl_svd<double> svd( A, 1.0e-8 );
  // Rank of (param_dof() -1) is an exact fit
  if ( (unsigned int)svd.rank() < param_dof() - 1 ) {
    std::cerr << "rrel_orthogonal_regression::WeightedLeastSquaresFit --- singularity!\n";
    return false;
  }
  else {
    vnl_vector<double> norm = svd.nullvector();
    params.set_size(norm.size()+1);
    for (unsigned int i=0; i<norm.size(); ++i) {
      params[i] = norm[i];
    }
    params[norm.size()] = -1*dot_product(norm, avg);
    return true;
  }
}


void
rrel_orthogonal_regression::print_points() const
{
  std::cout << "\nrrel_orthogonal_regression::print_points:\n"
           << "  param_dof() = " << param_dof() << '\n'
           << "  num_pts = " << vars_.rows() << "\n\n"
           << " i   vars_\n"
           << " =   =========\n";
  for ( unsigned int i=0; i<vars_.rows(); ++i ) {
    std::cout << ' ' << i << "   " << vars_.get_row (i) << '\n';
  }
}
