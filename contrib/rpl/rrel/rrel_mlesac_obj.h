#ifndef rrel_mlesac_obj_h_
#define rrel_mlesac_obj_h_
//:
//  \file
//  \author Bess Lee (leey@cs.rpi.edu)
//  \date   Oct 2001
//
//  MLESAC objective function.
//
//  \verbatim
//  Modifications:
//     25 Oct 2001  Amitha Perera: added comments
//  \endverbatim

#include <rrel/rrel_objective.h>

//: MLESAC objective function.
//  Implements the MLESAC objective as presented in Torr and
//  Zisserman, CVIU 2000. The "cost" is the negative log likelihood of
//  the residuals, assuming that each residual is i.i.d. with a
//  mixture distribution (zero-mean Gaussian modeling the inliers and
//  uniform distribution modeling the outliers). The standard
//  deviation of the Gaussian is the prior scale supplied by the
//  problem or set in the search technique. The width of the uniform
//  distribution is given by \a outlier_sigma. The mixing parameter is
//  estimated via the EM algorithm.
//
//  \a residual_dof is the number of "error random variables" are
//  combined in computing the residual. In the Torr and Zisserman
//  paper, for example, the residual for homography estimation results
//  from the combination of four errors: one in each coordinate of
//  each point in the correspondence pair. In general, this should be
//  set to rrel_estimation_problem::residual_dof().

class rrel_mlesac_obj : public rrel_objective {
public:
  //: Constructor.
  //  \a residual_dof is the number of "error random variables" are
  //  combined in computing the residual. In general, this should be
  //  set to rrel_estimation_problem::residual_dof(). \a outlier_sigma
  //  (\f$ os \f$) is the width of the outlier uniform distribution,
  //  so that each residual has a \f$ (1-\gamma)/{os} \f$ probability of
  //  being an outlier, where \f$ \gamma \f$ is the fraction of
  //  inliers. \a outlier_frac is the initial value of the mixing
  //  parameter for the EM algorithm. It can safely be left at the
  //  default.
  rrel_mlesac_obj(unsigned int residual_dof, double outlier_sigma = 20.0, double outlier_frac = 0.5 );

  //: Destructor.
  ~rrel_mlesac_obj() override = default;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn(vect_const_iter res_begin, vect_const_iter res_end,
                     vect_const_iter scale_begin,
                     vnl_vector<double>* param_vector=nullptr ) const override;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn(vect_const_iter begin, vect_const_iter end,
                     double scale,
                     vnl_vector<double>* param_vector=nullptr ) const override;

  //: True.
  //  The MLESAC algorithm is sensitive to the scale, and requires a prior scale.
  bool requires_prior_scale() const override
    { return true; }

protected:
  double outlier_sigma_;
  double outlier_frac_;
  unsigned int residual_dof_;
};

#endif // rrel_mlesac_obj_h_
