// This is gel/vdgl/vdgl_OrthogRegress.h
#ifndef vdgl_OrthogRegress_h_
#define vdgl_OrthogRegress_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
// .NAME vdgl_OrthogRegress
// .INCLUDE vdgl/vdgl_OrthogRegress.h
// .FILE vdgl_OrthogRegress.cxx
// \author fsm@robots.ox.ac.uk

#include <vcl_cmath.h>
#include <vdgl/vdgl_ortho_regress.h>

class vdgl_OrthogRegress : public vdgl_ortho_regress
{
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

 private:
  double a_, b_, c_;
  double est_;
};

#endif // vdgl_OrthogRegress_h_
