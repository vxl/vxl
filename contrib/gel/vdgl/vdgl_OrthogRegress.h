// This is gel/vdgl/vdgl_OrthogRegress.h
#ifndef vdgl_OrthogRegress_h_
#define vdgl_OrthogRegress_h_
//:
// \file
// \author fsm
//
// \verbatim
//  Modifications
//   10 Sep. 2004 Peter Vanroose  Inlined all 1-line methods in class decl
// \endverbatim

#include <vcl_cmath.h>
#include <vdgl/vdgl_ortho_regress.h>

class vdgl_OrthogRegress : public vdgl_ortho_regress
{
  double a_, b_, c_;
  double est_;
 public:
  typedef vdgl_ortho_regress base;
  vdgl_OrthogRegress() { }
  vdgl_OrthogRegress(double, double, double) { } // FIXME
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
};

#endif // vdgl_OrthogRegress_h_
