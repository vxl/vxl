#ifndef rrel_kernel_density_obj_h_
#define rrel_kernel_density_obj_h_
//:
//  \file
//  \brief Kernel Density objective function
//  \author Ying-Lin Bess Lee (leey@cs.rpi.edu)
//  \date Aug 2002

#include <rrel/rrel_objective.h>

enum rrel_kernel_scale_type { RREL_KERNEL_MAD, RREL_KERNEL_PRIOR, RREL_KERNEL_MUSE };


//: Kernel Density objective function.
//  Implements the Kernel Density Estimation as presented in the
//  paper "Robust Computer Vision through Kernel Density" by Chen
//  and Meer, 2002.
//  Given residuals ri, i = 1,...,n, the cost function is the estimated
//  density f(x) based on a kernel function K(u) and a bandwidth h as
//  f(x) = -1 / (nh) * sum( K(u) )
//  where
//  u = (ri-x)/h
//  K(u) = 1.09375 * (1 - u^2)^3
//  h = [243 * R(K) / 35 / Mu(K)^2 / n]^0.2 * scale
//  The scale can be provided as a prior scale, or computed by MAD or MUSE.

class rrel_kernel_density_obj : public rrel_objective
{
 public:
  //: Constructor.
  rrel_kernel_density_obj(rrel_kernel_scale_type scale_type=RREL_KERNEL_MAD);

  //: Destructor.
  virtual ~rrel_kernel_density_obj() {}

  //: Evaluate the objective function on heteroscedastic residuals.
  //  Not implemented.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* param_vector=0 ) const;

  //: Evaluate the objective function on homoscedastic residuals.
  //  prior_scale is needed if the type RREL_KERNEL_PRIOR is used.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      double prior_scale = 0,
                      vnl_vector<double>* = 0) const;

  //: Set the type of the scale.
  //  RREL_KERNEL_MAD uses median absolute deviations to estimate the scale.
  //  RREL_KERNEL_PRIOR uses the prior scale provided.
  //  RREL_KERNEL_MUSE uses MUSE to estimate the scale.
  virtual void set_scale_type( rrel_kernel_scale_type t = RREL_KERNEL_MAD )
  { scale_type_ = t; }

  //: Depends on the scale type used.
  //  \sa rrel_objective::requires_prior_scale.
  virtual bool requires_prior_scale() const
  { if (scale_type_ == RREL_KERNEL_PRIOR) return true; return false; }

  //: x is set to 0;
  void fix_x() { fix_x_ = true; }

  //: The mode of the density estimate which maximizes the estimated kernel density.
  //  The value can be used to shift the estimated parameters.
  double best_x( vect_const_iter res_begin, vect_const_iter res_end,
                 double scale = 0 ) const;
 private:

  //: Calculate the bandwidth.
  double bandwidth(vect_const_iter res_begin, vect_const_iter res_end,
                   double prior_scale) const;

  //: Given a kernel and the bandwidth, the estimated density of residuals.
  double kernel_density(vect_const_iter res_begin, vect_const_iter res_end,
                        double x, double h) const;

  //: Kernel function K(u).
  double kernel_function(double u) const;

  rrel_kernel_scale_type scale_type_;

  bool fix_x_;
};

#endif
