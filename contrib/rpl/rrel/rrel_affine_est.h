#ifndef rrel_affine_est_h_
#define rrel_affine_est_h_

//:
// \file
// \author Gehua yang
// \date Oct 2004
// Class to maintain data and optimization model for affine transformation estimation

#include <rrel/rrel_estimation_problem.h>
#include <rrel/rrel_linear_regression.h>
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <vcl_vector.h>

class rrel_affine_est : public rrel_estimation_problem {
public:
  typedef rrel_linear_regression* regression_ptr;
  //: Constructor that includes all information in the sample vectors.
  //  For each sample, the first m-1 entries are the independent
  //  variables, and the last entry is the dependent variable.
  rrel_affine_est( const vcl_vector< vgl_point_2d<double> > & from_pts,
                   const vcl_vector< vgl_point_2d<double> > & to_pts );

  //: Constructor with data pre-separated into arrays of independent and dependent variables. 
  rrel_affine_est( const vcl_vector< vnl_vector<double> > & from_pts,
                   const vcl_vector< vnl_vector<double> > & to_pts,
                   unsigned int dim = 2 );

  //: Destructor.
  virtual ~rrel_affine_est();

  //: Total number of data points.
  unsigned int num_samples( ) const; 

  //: organize into usual representation
  vnl_vector<double> 
  trans( const vnl_vector<double>& params ) const;

  vnl_matrix<double> 
  A( const vnl_vector<double>& params ) const;

  //: Generate a parameter estimate from a minimal sample set.
  bool fit_from_minimal_set( const vcl_vector<int>& point_indices,
                             vnl_vector<double>& params ) const;

  //: Compute signed fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          vcl_vector<double>& residuals ) const;

  //: \brief Weighted least squares parameter estimate. 
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const vcl_vector<double>* weights=0 ) const;

protected:
  
  vcl_vector< vnl_vector<double> > from_pts_;
  vcl_vector< vnl_vector<double> > to_pts_;

  unsigned affine_dof_;
  unsigned min_num_pts_;
  unsigned num_samples_;
};
  
#endif
