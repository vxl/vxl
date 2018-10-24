#ifndef rrel_wls_obj_h_
#define rrel_wls_obj_h_

//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
//  Abstract base class for objective functions that can be used with
//  IRLS-type searches.

#include <rrel/rrel_objective.h>

//: Abstract base class for objective functions that can compute weights.
//  This class of objective functions, in addition to providing a
//  "cost" for the estimate, can provide a weight for each
//  residual. The weights are used by some search techniques, such as
//  IRLS. Most commonly used M-estimator objective functions can
//  provide a weights, and hence will be a descendant of this
//  class. (See Stewart, "Robust Parameter Estimation in Computer
//  Vision", SIAM Reviews 41, Sept 1999.)

class rrel_wls_obj : public rrel_objective {
public:
  //: Constructor.
  rrel_wls_obj() = default;

  //: Destructor.
  ~rrel_wls_obj() override = default;

  //: Evaluate the objective function on heteroscedastic residuals.
  // This version is used for heteroscedastic data, where each
  // residual has its own scale. The number of scale values must, of
  // course, equal the number of residuals.
  //
  // The weights (one per residual) are returned using \a wgt_begin,
  // which should point to an appropriately sized container.
  virtual void wgt( vect_const_iter res_begin, vect_const_iter res_end,
                    vect_const_iter scale_begin,
                    vect_iter wgt_begin ) const = 0;

  //: Computes the weights for homoscedastic residuals.
  // This version is used for homoscedastic data, where each residual
  // is distributed with a common scale. (See also the comments in
  // rrel_objective::fcn.)
  //
  // The weights (one per residual) are returned using \a wgt_begin,
  // which should point to an appropriately sized container.
  virtual void wgt( vect_const_iter begin, vect_const_iter end,
                    double scale,
                    vect_iter wgt_begin ) const = 0;

  //: The weight of the residual.
  //  \a u is a normalised residual (i.e. u=r/scale). wgt(u) is
  //  normally \f$ (1/u) (\partial{\rho} / \partial{u}) \f$.
  virtual double wgt( double u ) const = 0;
};

#endif // rrel_wls_obj_h_
