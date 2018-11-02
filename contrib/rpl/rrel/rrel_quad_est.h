#ifndef rrel_quad_est_h_
#define rrel_quad_est_h_

//:
// \file
// \author Gehua yang
// \date Oct 2006
// Class to maintain data and optimization model for quadratic transformation estimation

#include <iostream>
#include <vector>
#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_linear_regression.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

class rrel_quad_est : public rrel_estimation_problem {
public:
  typedef rrel_linear_regression* regression_ptr;
  //: Constructor that includes all information in the sample vectors.
  //  For each sample, the first m-1 entries are the independent
  //  variables, and the last entry is the dependent variable.
  rrel_quad_est( const std::vector< vgl_point_2d<double> > & from_pts,
                   const std::vector< vgl_point_2d<double> > & to_pts );

  //: Constructor with data pre-separated into arrays of independent and dependent variables.
  rrel_quad_est( const std::vector< vnl_vector<double> > & from_pts,
                   const std::vector< vnl_vector<double> > & to_pts,
                   unsigned int dim = 2 );

  //: Destructor.
  ~rrel_quad_est() override;

  //: Total number of data points.
  unsigned int num_samples( ) const override;

  //: organize into usual representation
  vnl_vector<double>
  trans( const vnl_vector<double>& params ) const;

  vnl_matrix<double>
  A( const vnl_vector<double>& params ) const;

  //: Generate a parameter estimate from a minimal sample set.
  bool fit_from_minimal_set( const std::vector<int>& point_indices,
                             vnl_vector<double>& params ) const override;

  //: Compute signed fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          std::vector<double>& residuals ) const override;

  //: \brief Weighted least squares parameter estimate.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const std::vector<double>* weights=nullptr ) const override;

protected:

  std::vector< vnl_vector<double> > from_pts_;
  std::vector< vnl_vector<double> > to_pts_;

  unsigned quad_dof_;
  unsigned dim_;
  unsigned min_num_pts_;
  unsigned num_samples_;
};

#endif
