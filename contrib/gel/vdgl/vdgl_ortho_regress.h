// This is gel/vdgl/vdgl_ortho_regress.h
#ifndef vdgl_ortho_regress_h_
#define vdgl_ortho_regress_h_
// .NAME vdgl_ortho_regress
// .INCLUDE vdgl/vdgl_ortho_regress.h
// .FILE vdgl_ortho_regress.cxx
//
// .SECTION Description
// A vdgl_ortho_regress object performs least-squares
// fitting of straight lines to point sets.
//
// Internally, the object maintains only the moments
//   S_{mn} = \sum_i x_i^m y_i^n
// and this is sufficient for both unconstrained and
// constrained (fit line through a point) fitting.
//

class vdgl_ortho_regress
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

  vdgl_ortho_regress() { reset(); }

  ~vdgl_ortho_regress() = default;

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

#endif // vdgl_ortho_regress_h_
