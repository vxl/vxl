#ifndef fsm_ortho_regress_h_
#define fsm_ortho_regress_h_
#ifdef __GNUC__
#pragma interface
#endif
/*
  fsm@robots.ox.ac.uk
*/

class fsm_ortho_regress {
public:
  unsigned S1;
  double Sx, Sy;
  double Sxx, Sxy, Syy;

  void reset() {
    S1 = 0;
    Sx = Sy = 0;
    Sxx = Sxy = Syy =0;
  }
  
  fsm_ortho_regress() { reset(); }

  ~fsm_ortho_regress() { }

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

#endif
