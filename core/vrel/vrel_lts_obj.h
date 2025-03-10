#ifndef vrel_lts_obj_h_
#define vrel_lts_obj_h_
//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// \brief The Least-Trimmed-Squares (LTS) objective function

#include <vrel/vrel_objective.h>

//: The Least-Trimmed-Squares (LTS) objective function.
//  The Least-Trimmed-of-Squares algorithm was defined in a 1984
//  Journal of the American Statistical Association paper by Peter
//  Rousseeuw (vol 79, pp 871-880).  (See Stewart, "Robust Parameter
//  Estimation in Computer Vision", SIAM Reviews 41, Sept 1999.) The
//  class implemented here gives the objective function for that
//  algorithm.  It is computed from a set of (signed) residuals.  The
//  objective function is the sum of the squares of the first
//  (increased order) inlier_frac squared residuals.  The default
//  inlier_frac is 0.5, but it could be any value.  The search
//  algorithm is implemented in vrel_ran_sam_search.

class vrel_lts_obj : public vrel_objective
{
public:
  //: Constructor.
  //  \a num_sam_inst is the minimum number of samples needed for
  //  a unique parameter estimate. That is, num_sam_inst should be set
  //  to vrel_estimation_problem::num_samples_to_instantiate(). \a
  //  inlier_frac is the minimum expected number of inlier
  //  residuals. In general, \a inlier_frac should be 1 minus the
  //  maximum expected fraction of outliers. If the maximum expected
  //  fraction of outliers is not known, then the MUSE objective
  //  function should be used.
  vrel_lts_obj(unsigned int num_sam_inst, double inlier_frac = 0.5);

  ~vrel_lts_obj() override;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa vrel_objective::fcn.
  double
  fcn(vect_const_iter res_begin,
      vect_const_iter res_end,
      vect_const_iter /* scale is unused */,
      vnl_vector<double> * = nullptr /* param vector is unused */) const override;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa vrel_objective::fcn.
  double
  fcn(vect_const_iter begin,
      vect_const_iter end,
      double = 0 /* scale is unused */,
      vnl_vector<double> * = nullptr /* param vector is unused */) const override;

  //: False.
  //  The LTS objective is based on order statistics, and does not
  //  require any scale parameter, estimated or otherwise.
  bool
  requires_prior_scale() const override
  {
    return false;
  }

protected:
  //: Number of samples needed for a unique fit = number of dependent residuals.
  unsigned int num_sam_inst_;
  double inlier_frac_;
};

#endif
