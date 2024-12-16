#ifndef vrel_kernel_density_obj_h_
#define vrel_kernel_density_obj_h_
//:
//  \file
//  \brief Kernel Density objective function
//  \author Ying-Lin Bess Lee (leey@cs.rpi.edu)
//  \date Aug 2002

#include <vrel/vrel_objective.h>

enum vrel_kernel_scale_type
{
  VREL_KERNEL_MAD,
  VREL_KERNEL_PRIOR,
  VREL_KERNEL_MUSE
};


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

class vrel_kernel_density_obj : public vrel_objective
{
public:
  //: Constructor.
  vrel_kernel_density_obj(vrel_kernel_scale_type scale_type = VREL_KERNEL_MAD);

  //: Destructor.
  ~vrel_kernel_density_obj() override = default;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  Not implemented.
  //  \sa vrel_objective::fcn.
  double
  fcn(vect_const_iter res_begin,
      vect_const_iter res_end,
      vect_const_iter scale_begin,
      vnl_vector<double> * param_vector = nullptr) const override;

  //: Evaluate the objective function on homoscedastic residuals.
  //  prior_scale is needed if the type VREL_KERNEL_PRIOR is used.
  //  \sa vrel_objective::fcn.
  double
  fcn(vect_const_iter res_begin,
      vect_const_iter res_end,
      double prior_scale = 0,
      vnl_vector<double> * = nullptr) const override;

  //: Set the type of the scale.
  //  VREL_KERNEL_MAD uses median absolute deviations to estimate the scale.
  //  VREL_KERNEL_PRIOR uses the prior scale provided.
  //  VREL_KERNEL_MUSE uses MUSE to estimate the scale.
  virtual void
  set_scale_type(vrel_kernel_scale_type t = VREL_KERNEL_MAD)
  {
    scale_type_ = t;
  }

  //: Depends on the scale type used.
  //  \sa vrel_objective::requires_prior_scale.
  bool
  requires_prior_scale() const override
  {
    return scale_type_ == VREL_KERNEL_PRIOR;
  }

  //: x is set to 0;
  void
  fix_x()
  {
    fix_x_ = true;
  }

  //: The mode of the density estimate which maximizes the estimated kernel density.
  //  The value can be used to shift the estimated parameters.
  double
  best_x(vect_const_iter res_begin, vect_const_iter res_end, double scale = 0) const;

private:
  //: Calculate the bandwidth.
  double
  bandwidth(vect_const_iter res_begin, vect_const_iter res_end, double prior_scale) const;

  //: Given a kernel and the bandwidth, the estimated density of residuals.
  double
  kernel_density(vect_const_iter res_begin, vect_const_iter res_end, double x, double h) const;

  //: Kernel function K(u).
  double
  kernel_function(double u) const;

  vrel_kernel_scale_type scale_type_;

  bool fix_x_{ false };
};

#endif
