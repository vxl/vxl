// This is oxl/osl/osl_OrthogRegress.h
#ifndef osl_OrthogRegress_h_
#define osl_OrthogRegress_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME osl_OrthogRegress
// .INCLUDE osl/osl_OrthogRegress.h
// .FILE osl_OrthogRegress.cxx
// \author fsm

#include <vcl_cmath.h>
#include <osl/osl_hacks.h>
#include <osl/osl_ortho_regress.h>

class osl_OrthogRegress : public osl_ortho_regress
{
 public:
  typedef osl_ortho_regress base;
  osl_OrthogRegress() { }
  osl_OrthogRegress(double, double, double) { } // FIXME
  void IncrByXY(double x, double y) { base::add_point(x, y); }
  void DecrByXY(double x, double y) { base::remove_point(x, y); }
  void Fit() { base::fit(a_, b_, c_); double r = vcl_sqrt(a_*a_ + b_*b_); a_/=r; b_/=r; c_/=r; }
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

#endif // osl_OrthogRegress_h_
