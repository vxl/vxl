// This is oxl/vgui/internals/vgui_un_project.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_un_project.h"

#include <vnl/vnl_vector.h>
#include <vnl/vnl_matrix.h>
#include <vnl/algo/vnl_svd.h>

bool vgui_un_project(double const * const *H,
                     double const X[4],
                     double Y[4])
{
  vnl_matrix<double> M(4, 4);
  M.set_row(0, H[0]);
  M.set_row(1, H[1]);
  M.set_row(2, H[2]);
  M.set_row(3, H[3]);
  vnl_svd<double> svd(M);
  vnl_vector<double> p(X, 4);
  vnl_vector<double> q = svd.solve(p);
  q.copy_out(Y);
  return true;
}

bool vgui_un_project(double const H[4][4], double const X[4], double Y[4]) {
  double const *H_[4] = { H[0], H[1], H[2], H[3] };
  return vgui_un_project(H_, X, Y);
}

bool vgui_un_project(double const H[16], double const X[4], double Y[4]) {
  double const *H_[4] = { H, H+4, H+8, H+12 };
  return vgui_un_project(H_, X, Y);
}
