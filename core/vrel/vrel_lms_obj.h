#ifndef vrel_lms_obj_h_
#define vrel_lms_obj_h_
//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// \brief The Least-Median-of-Squares (LMS) objective function
//
// \verbatim
//   2004-04, Charlene Tsai (tsaic@cs.rpi.edu). Added scale(.) function.
// \endverbatim

#include <vrel/vrel_objective.h>

//: The Least-Median-of-Squares (LMS) objective function.
//
//  The Least-Median-of-Squares algorithm was defined in a 1984
//  Journal of the American Statistical Association paper by Peter
//  Rousseeuw (vol 79, pp 871-880).  (See Stewart, "Robust Parameter
//  Estimation in Computer Vision", SIAM Reviews 41, Sept 1999.)  The
//  class implemented here gives the objective function for that
//  algorithm. The LMS objective function is the median of the squared
//  residuals. This class is generalised to allow any inlier fraction
//  (not just the median) from the order statistics.

class vrel_lms_obj : public vrel_objective
{
public:
  //: Constructor.
  //  \a num_sam_inst is the minimum number of samples needed for
  //  a unique parameter estimate. That is, num_sam_inst should be set
  //  to vrel_estimation_problem::num_samples_to_instantiate(). \a
  //  inlier_frac is the minimum expected number of inlier
  //  residuals. The default value of 0.5 makes this the standard
  //  median objective function. In general, it should be 1 minus the
  //  maximum expected fraction of outliers. If the maximum expected
  //  fraction of outliers is not known, then the MUSE objective
  //  function should be used.
  vrel_lms_obj(unsigned int num_sam_inst, double inlier_frac = 0.5);

  ~vrel_lms_obj() override;

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
  //  The LMS objective is based on order statistics, and does not
  //  require any scale parameter, estimated or otherwise.
  bool
  requires_prior_scale() const override
  {
    return false;
  }

  //: True. The scale is estimated as MAD (Median Absolute Deviation)
  //  \sa vrel_objective::can_estimate_scale.
  bool
  can_estimate_scale() const override
  {
    return true;
  }

  //: Scale estimate (median absolute deviation -- MAD).
  //  \sa vrel_util_median_abs_dev_scale(.)
  double
  scale(vect_const_iter res_begin, vect_const_iter res_end) const override;

protected:
  //: Number of samples needed for a unique fit = number of dependent residuals.
  unsigned int num_sam_inst_;
  double inlier_frac_;
};

#endif // vrel_lms_obj_h_
