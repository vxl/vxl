#ifndef vsl_OrthogRegress_h_
#define vsl_OrthogRegress_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

#include <vcl/vcl_cmath.h>
#include <vsl/vsl_hacks.h>
#include <vsl/vsl_ortho_regress.h>

class vsl_OrthogRegress : public vsl_ortho_regress {
public:
  typedef vsl_ortho_regress base;
  vsl_OrthogRegress() { }
  vsl_OrthogRegress(double, double, double) { } // FIXME
  void IncrByXY(double x, double y) { base::add_point(x, y); }
  void DecrByXY(double x, double y) { base::remove_point(x, y); }
  void Fit() { base::fit(a_, b_, c_); double r = sqrt(a_*a_ + b_*b_); a_/=r; b_/=r; c_/=r; }
  double GetA() const { return a_; }
  double GetB() const { return b_; }
  double GetC() const { return c_; }
  double GetCost() const { return base::cost(a_, b_, c_); }
  double GetEstCost() const { return est_; }
  void SetEstCost(double v) { est_ = v; }
  void Reset() { base::reset(); }

private:
  double a_, b_, c_;
  double est_;
};


#endif
