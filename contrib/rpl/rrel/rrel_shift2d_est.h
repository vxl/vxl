#ifndef rrel_shift2d_est_h_
#define rrel_shift2d_est_h_

//:
// \file
// \author Fred Wheeler
// \date October 2002
// \brief Class to maintain data and optimization model for 2d shift estimation

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>
#include <vnl/vnl_fwd.h>
#include <rrel/rrel_estimation_problem.h>

//: Class to maintain data and optimization model for 2d shift estimation.
//
// This class is an adaptation of rrel_homography2d_est to compute a
// 2D shift instead of a 2D homography.

class rrel_shift2d_est : public rrel_estimation_problem {
public:

  //: Constructor from vgl_homg_point_2d's
  rrel_shift2d_est( const std::vector< vgl_homg_point_2d<double> > & from_pts,
                    const std::vector< vgl_homg_point_2d<double> > & to_pts );

  //: Constructor from vnl_vectors
  rrel_shift2d_est( std::vector< vnl_vector<double> >  from_pts,
                    std::vector< vnl_vector<double> >  to_pts );

  //: Destructor.
  ~rrel_shift2d_est() override;

  //: Total number of correspondences.
  unsigned int num_samples( ) const override;

  //: The degrees of freedom in the residual.  Each coordinate has
  // Gaussian error, so the Euclidean distance residual has 2 degrees
  // of freedom.
  unsigned int residual_dof() const override { return 2; }

  //: Generate a parameter estimate from a minimal sample.
  bool fit_from_minimal_set( const std::vector<int>& point_indices,
                             vnl_vector<double>& params ) const override;

  //: Compute unsigned fit residuals relative to the parameter estimate.
  void compute_residuals( const vnl_vector<double>& params,
                          std::vector<double>& residuals ) const override;

  //: Weighted least squares parameter estimate.  The normalized covariance is not yet filled in.
  bool weighted_least_squares_fit( vnl_vector<double>& params,
                                   vnl_matrix<double>& norm_covar,
                                   const std::vector<double>* weights=nullptr ) const override;

protected:
  std::vector< vnl_vector< double > > del_pts_;
  void compute_del_pts ();

protected:
  std::vector< vnl_vector< double > > from_pts_;
  std::vector< vnl_vector< double > > to_pts_;
};

#endif // rrel_shift2d_est_h_
