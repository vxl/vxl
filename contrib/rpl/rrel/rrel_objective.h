#ifndef rrel_objective_h_
#define rrel_objective_h_
//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// \brief Abstract base class for robust objective functions.

#include <iostream>
#include <vector>
#include <vnl/vnl_fwd.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: An objective function to be minimised.
//  It returns a "cost" given the residuals, and thus gives a cost for
//  the estimate.
//
//  Robust objective functions will use the residuals to determine
//  which points are inliers and which are outliers, and will
//  downgrade the influence of the of those samples.

class rrel_objective
{
 public:
  //: The iterators used to pass in values.
  //  Since we don't allow member templates, we have to fix on a
  //  particular type of container for residuals. Using this typedef
  //  will allow things to easily change when member templates are
  //  allowed.
  typedef std::vector<double>::const_iterator vect_const_iter;

  //: The iterators used to pass out values.
  typedef std::vector<double>::iterator vect_iter;

 public:
  rrel_objective() = default;
  virtual ~rrel_objective() = default;

  //: Evaluate the objective function on heteroscedastic residuals.
  // This version is used for heteroscedastic data, where each
  // residual has its own scale.  Some objective functions, such as
  // M-estimators, will require a scale value.  Others, such as Least
  // Median of Squares (LMS) with intercept adjustment, will require
  // access to the parameter vector.
  //
  // The number of scale values must, of course, equal the number of
  // residuals.
  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* param_vector ) const = 0;

  //: Evaluate the objective function on homoscedastic residuals.
  // This version is used for homoscedastic data, where each residual
  // is distributed with a common scale.  Some objective functions,
  // such as M-estimators, will require a scale value.  Others, such
  // as Least Median of Squares (LMS) with intercept adjustment, will
  // require access to the parameter vector.
  //
  // Using the previous function for homoscedastic data would imply
  // the creation of a vector of equal values. Since the majority of
  // problems assume homoscedastic data, a "convenience" function that
  // avoids the scale vector is useful.
  virtual double fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* param_vector ) const = 0;

  //: True if the objective function must have a prior scale.
  //  For some objective functions, such as RANSAC, an estimated scale
  //  is not enough. The problem must have a prior scale estimate.
  virtual bool requires_prior_scale() const = 0;

  //: True if the objective function can estimate scale.
  //  Some objective functions, such as MUSE, can provide an accurate
  //  inlier scale estimate.
  virtual bool can_estimate_scale() const { return false; }

  //: Scale estimate.
  //  The result is undefined if can_estimate_scale() is false.
  virtual double scale( vect_const_iter /*res_begin*/, vect_const_iter /*res_end*/ ) const { return 0.0; }
};

#endif
