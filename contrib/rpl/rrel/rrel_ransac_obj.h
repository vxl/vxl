#ifndef rrel_ransac_obj_h_
#define rrel_ransac_obj_h_

//:
// \file
// \author Chuck Stewart (stewart@cs.rpi.edu)
// RANSAC objective function.

#include <rrel/rrel_objective.h>

//: The objective function for RANSAC.
//  The RANSAC objective function is 0 for residuals whose magnitude
//  is less than a threshold, and 1 otherwise. This loss function is
//  really here only for historical purposes and should not be used in
//  practice.  Smooth loss functions should be used.  This is shown in
//  a practical sense in Torr and Zisserman, CVIU, April 2000 and in a
//  theoretical sense in Stewart, PAMI, August 1997.

class rrel_ransac_obj : public rrel_objective {
public:
  //: Constructor.
  //  The threshold is scale_mult*prior_scale, where prior scale is
  //  supplied by the problem.
  rrel_ransac_obj( double scale_mult = 2.0 );

  //: Destructor.
  ~rrel_ransac_obj() override;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* = nullptr /* param vector is unused */ ) const override;

  //: Evaluate the objective function on homoscedastic residuals.
  //  \sa rrel_objective::fcn.
  double fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* = nullptr /* param vector is unused */ ) const override;

  //: True.
  //  Using a RANSAC objective with an estimated scale doesn't make
  //  sense, because the any scale estimate tends to be inaccurate and
  //  RANSAC is sensitive to the threshold (and hence the scale).
  bool requires_prior_scale() const override
    { return true; }

protected:
  double scale_mult_;
};

#endif
