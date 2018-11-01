// This is oxl/osl/osl_ortho_regress.h
#ifndef osl_ortho_regress_h_
#define osl_ortho_regress_h_
// .NAME osl_ortho_regress
// .INCLUDE osl/osl_ortho_regress.h
// .FILE osl_ortho_regress.cxx
//
// .SECTION Description
// An osl_ortho_regress object performs least-squares
// fitting of straight lines to point sets.
//
// Internally, the object maintains only the moments
//   S_{mn} = \sum_i x_i^m y_i^n
// and this is sufficient for both unconstrained and
// constrained (fit line through a point) fitting.
//
// There is no need to store any fitting costs, "current
// fit" etc on this object.
//
// \author fsm

class osl_ortho_regress
{
 public:
  unsigned S1;                  // S_00
  double Sx, Sy;                // S_10, S_01
  double Sxx, Sxy, Syy;         // S_20, S_11, S_02

  void reset() {
    S1 = 0;
    Sx = Sy = 0;
    Sxx = Sxy = Syy =0;
  }

  osl_ortho_regress() { reset(); }

  ~osl_ortho_regress() = default;

  void add_point(double x, double y) {
    ++S1;
    Sx += x; Sy += y;
    Sxx += x*x; Sxy += x*y; Syy += y*y;
  }
  void add_points(double const *, double const *, unsigned);
  void add_points(float  const *, float  const *, unsigned);

  void remove_point(double x, double y) {
    --S1;
    Sx -= x; Sy -= y;
    Sxx -= x*x; Sxy -= x*y; Syy -= y*y;
  }
  void remove_points(double const *, double const *, unsigned);
  void remove_points(float  const *, float  const *, unsigned);

  double cost(double a, double b, double c) const;
  double rms_cost(double a, double b, double c) const;

  // by address (preferred)
  bool fit(double *a, double *b, double *c) const
    { return fit(*a, *b, *c); }
  bool fit_constrained(double x, double y, double *a, double *b, double *c) const
    { return fit_constrained(x, y, *a, *b, *c); }

 protected:
  // by reference
  bool fit(double &a, double &b, double &c) const;
  bool fit_constrained(double x, double y, double &a, double &b, double &c) const;
};

#endif // osl_ortho_regress_h_
