#ifndef rrel_ransac_obj_smooth_cost_h_
#define rrel_ransac_obj_smooth_cost_h_

#include <rrel/rrel_ransac_obj.h>
//:
// \file
// \author Sancar Adali (sancar_adali@brown.edu)
// RANSAC objective function with smooth cost function.

class rrel_ransac_obj_smooth_cost : public rrel_ransac_obj
{
 protected:
  double scale_mult_;
 public:
  rrel_ransac_obj_smooth_cost(double scale_mult = 2.0)
  : rrel_ransac_obj(scale_mult), scale_mult_( scale_mult ) {}

  virtual ~rrel_ransac_obj_smooth_cost() {}

  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* param_vector = 0 ) const;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* param_vector = 0 ) const;

  //: True.
  //  Using a RANSAC objective with an estimated scale doesn't make
  //  sense, because the any scale estimate tends to be inaccurate and
  //  RANSAC is sensitive to the threshold (and hence the scale).
  virtual bool requires_prior_scale() const { return true; }
};

#endif
