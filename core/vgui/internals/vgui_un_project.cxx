// This is core/vgui/internals/vgui_un_project.cxx
//:
// \file
// \author fsm

#include "vgui_un_project.h"

#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_inverse.h>

bool vgui_un_project(double const * const *H,
                     double const X[4],
                     double Y[4])
{
  vnl_matrix_fixed<double,4,4> M;
  M.set_row(0, H[0]);
  M.set_row(1, H[1]);
  M.set_row(2, H[2]);
  M.set_row(3, H[3]);
  vnl_vector_fixed<double,4> p(X);
  vnl_vector_fixed<double,4> q = vnl_inverse(M)*p;
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
