// This is gel/vdgl/vdgl_ortho_regress.cxx
//:
//  \file

#include <iostream>
#include <cmath>
#include "vdgl_ortho_regress.h"
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

// convenience methods

void vdgl_ortho_regress::add_points(double const *x, double const *y, unsigned n) {
  for (unsigned i=0; i<n; ++i)
    add_point(x[i], y[i]);
}

void vdgl_ortho_regress::add_points(float const *x, float const *y, unsigned n) {
  for (unsigned i=0; i<n; ++i)
    add_point(x[i], y[i]);
}

double vdgl_ortho_regress::rms_cost(double a, double b, double c) const {
  return std::sqrt( cost(a, b, c) / S1 );
}

//--------------------------------------------------------------------------------

// this can be done without netlib
#include <vnl/algo/vnl_symmetric_eigensystem.h>

// f(x, y)
// =
// a x^2 + 2 b x y + c y^2
// =
// [x y] [a b] [x]
//       [b c] [y]
static
bool symmetric_2x2_eigen_system(double a, double b, double c,
                                double *xmin, double *ymin, double *lmin,
                                double *xmax, double *ymax, double *lmax)
{
  vnl_matrix<double> M(2, 2);
  M(0, 0) = a;
  M(0, 1) = M(1, 0) = b;
  M(1, 1) = c;
  vnl_symmetric_eigensystem<double> sym(M);
  if (xmin) *xmin = sym.V(0, 0);
  if (ymin) *ymin = sym.V(1, 0);
  if (lmin) *lmin = sym.D(0, 0);

  if (xmax) *xmax = sym.V(0, 1);
  if (ymax) *ymax = sym.V(1, 1);
  if (lmax) *lmax = sym.D(1, 1);

  return true;
}

//:
// The problem is to minimize total squared distance to
// the line u.x + d = 0, ie. :
// minimize     \sum_i | u.x_i + d |^2
// subject to   |u|=1
//
// Writing x_i = xbar + y_i, where xbar is the centroid
// of the point set, the sum becomes
// \sum | u.(x_i - xbar) |^2 + S1 | u.xbar + d |^2
// , so that the optimal line must pass through the centroid.
// To find u, we find the eigenvector of the scatter matrix
// M = \sum (x_i - xbar)*(x_i - xbar)^t which has least
// eigenvalue.
//
double vdgl_ortho_regress::cost(double a, double b, double c) const {
  return
    (Sxx*a*a + 2*Sxy*a*b + Syy*b*b + 2*Sx*a*c + 2*Sy*b*c + S1*c*c)/(a*a+b*b);
}

bool vdgl_ortho_regress::fit(double &a, double &b, double &c) const {
  assert(S1 >= 2);
  double xbar = Sx/S1;
  double ybar = Sy/S1;

  if (!symmetric_2x2_eigen_system(Sxx-Sx*Sx/S1, Sxy-Sx*Sy/S1, Syy-Sy*Sy/S1,
                                  &a, &b, nullptr,
                                  nullptr, nullptr, nullptr))
    return false;

  c = -(a*xbar+b*ybar);
  return true;
}

//:
// The problem is to minimize total squared distance from
// a line passing through a given point x_0 :
//
// minimize     \sum_i | u.x_i + d |^2
// subject to   |u|=1
//              u.x_0 + d = 0
//
// But u.x_i + d = u.(x_i - x_0), so that we have to minimize
// u^t M u , where M = \sum_i (x_i - x_0)*(x_i - x_0)^t. This
// is solved by taking the eigenvector of M of least
// eigenvalue.
//
bool vdgl_ortho_regress::fit_constrained(double x, double y,
                                         double &a, double &b, double &c) const
{
  assert(S1 >= 1);

  if (!symmetric_2x2_eigen_system(Sxx-2*Sx*x+S1*x*x, Sxy-Sx*y-x*Sy+S1*x*y, Syy-2*Sy*y+S1*y*y,
                                  &a, &b, nullptr,
                                  nullptr, nullptr, nullptr))
    return false;

  c = -(a*x + b*y);
  return true;
}
