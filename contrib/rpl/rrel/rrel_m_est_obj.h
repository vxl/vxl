#ifndef rrel_m_est_obj_h_
#define rrel_m_est_obj_h_

//:
// \file
// \author Amitha Perera (perera@cs.rpi.edu)
//  Generic implementation for standard m-estimators.

#include <rrel/rrel_wls_obj.h>

//: Generic implementation for standard M-estimators.
//  Most standard M-estimators are defined by a rho and psi functions
//  on a normalised residual. This class allows derived classes to
//  merely implement those two functions, and implements the fcn() and
//  wgt() functions based on rho() and psi(). (See Stewart, "Robust
//  Parameter Estimation in Computer Vision", SIAM Reviews 41, Sept
//  1999.)

class rrel_m_est_obj : public rrel_wls_obj {
public:
  //: Constructor.
  rrel_m_est_obj() {}

  //: Destructor.
  virtual ~rrel_m_est_obj() {}


  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter res_begin, vect_const_iter res_end,
                      vect_const_iter scale_begin,
                      vnl_vector<double>* = 0 /* param vector is unused */ ) const;

  //: Evaluate the objective function on homoscedastic residuals.  
  //  \sa rrel_objective::fcn.
  virtual double fcn( vect_const_iter begin, vect_const_iter end,
                      double scale,
                      vnl_vector<double>* = 0 /* param vector is unused */ ) const;

  //: Evaluate the objective function on heteroscedastic residuals.
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter res_begin, vect_const_iter res_end,
                    vect_const_iter scale_begin,
                    vect_iter wgt_begin ) const;

  //: Computes the weights for homoscedastic residuals.  
  //  \sa rrel_wls_obj::wgt()
  virtual void wgt( vect_const_iter begin, vect_const_iter end,
                    double scale,
                    vect_iter wgt_begin ) const;

  //: The robust loss function for the M-estimator.
  //  \a u is a normalised residual (i.e. u=r/scale).
  virtual double rho( double u ) const = 0;

  //: The robust loss function for the M-estimator.
  //  \a r is the residual and \a s is the scale for that residual.
  inline  double rho( double r, double s ) const { return rho(r/s); }

  //: The weight of the residual.
  //  \a u is a normalised residual (i.e. u=r/scale). wgt(u) is
  //  normally \f$ (1/u) (\partial{\rho} / \partial{u}) \f$.
  virtual double wgt( double u ) const = 0;

  //: The weight of the residual.
  //  \a r is the residual and \a s is the scale for that residual.
  inline  double wgt( double r, double s ) const { return wgt(r/s)/(s*s); }

  //: False.
  //  In general, most M-estimators work quite well with an estimated
  //  scale. The scale estimate should be robust, but not necessarily
  //  efficient (e.g. MAD scale estimate.)
  virtual bool requires_prior_scale() const
    { return false; }

};

#endif
