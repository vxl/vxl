#ifndef rrel_lms_obj_h_
#define rrel_lms_obj_h_

//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// \brief The Least-Median-of-Squares (LMS) objective function
// 
// \verbatim
//   2004-04, Charlene Tsai (tsaic@cs.rpi.edu). Added scale(.) function.
// \endverbatim

#include <rrel/rrel_objective.h>

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

class rrel_lms_obj : public rrel_objective {
public:
  //: Constructor.
  //  \a num_sam_inst is the the minimum number of samples needed for
  //  a unique parameter estimate. That is, num_sam_inst should be set
  //  to rrel_estimation_problem::num_samples_to_instantiate(). \a
  //  inlier_frac is the minimum expected number of inlier
  //  residuals. The default value of 0.5 makes this the standard
  //  median objective function. In general, it should be 1 minus the
  //  maximum expected fraction of outliers. If the maximum expected
  //  fraction of outliers is not known, then the MUSE objective
  //  function should be used.
  rrel_lms_obj( unsigned int num_sam_inst, double inlier_frac=0.5 );

  ~rrel_lms_obj();

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter /* scale is unused */,
                      vnl_vector<double>* = 0 /* param vector is unused */ ) const;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter begin, vect_const_iter end,
                      double = 0 /* scale is unused */,
                      vnl_vector<double>* = 0 /* param vector is unused */ ) const;

  //: False.
  //  The LMS objective is based on order statistics, and does not
  //  require any scale parameter, estimated or otherwise.
  virtual bool requires_prior_scale() const
    { return false; }

  //: True. The scale is estimated as MAD (Median Absolute Deviation)
  //  \sa rrel_objective::can_estimate_scale.
  virtual bool can_estimate_scale() const { return true; }

  //: Scale estimate (median absolute deviation -- MAD).
  //  \sa rrel_util_median_abs_dev_scale(.)
  virtual double scale( vect_const_iter res_begin, vect_const_iter res_end ) const;

protected:
  //: Number of samples needed for a unique fit = number of dependent residuals.
  unsigned int num_sam_inst_;
  double inlier_frac_;
};

#endif // rrel_lms_obj_h_
