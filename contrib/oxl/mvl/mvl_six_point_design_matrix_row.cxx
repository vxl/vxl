// This is oxl/mvl/mvl_six_point_design_matrix_row.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cmath>
#include "mvl_six_point_design_matrix_row.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void mvl_six_point_design_matrix_row(vnl_matrix<double> const &A,
                                     vnl_matrix<double> const &B,
                                     double u, double v,
                                     double out_row[5])
{
  // Translate the last point (u, v) to the origin (0, 0):
  double CA[2][4], CB[2][4];
  for (int j=0; j<4; ++j) {
    CA[0][j] = A[0][j] - u * A[2][j];
    CA[1][j] = A[1][j] - v * A[2][j];
    //CA[2][j] = A[2][j];

    CB[0][j] = B[0][j] - u * B[2][j];
    CB[1][j] = B[1][j] - v * B[2][j];
    //CB[2][j] = B[2][j];
  }

  // Normalize CA, CB using magic.
  {
    double nn = 0;
    for (int i=0; i<2; ++i)
      for (int j=0; j<4; ++j)
        nn += CA[i][j] * CA[i][j] + CB[i][j] * CB[i][j];
    nn = std::sqrt(nn);

    for (int i=0; i<2; ++i) {
      for (int j=0; j<4; ++j) {
        CA[i][j] /= nn;
        CB[i][j] /= nn;
      }
    }
  }

  // The constraint on X (last point) is that [A X, B X, x] = 0. This means
  // that the last entry of the cross product of CA*X and CB*X is zero, and
  // can be written as Q(X) = Q(X, X) = 0 where the (asymmetric) matrix of
  // the quadric Q is computed as follows:
  double Q[4][4];
  for (int r=0; r<4; ++r)
    for (int s=0; s<4; ++s)
      Q[r][s] = CA[0][r]*CB[1][s] - CA[1][r]*CB[0][s];

  // The quadric Q vanishes on the five canonical base points, so
  // the quadratic expression 2 Q(X) is equivalent to a linear one
  // in the five variables
  //    (a, b, c, d, e) = \psi(X)
  // Compute the coefficients.
  out_row[0] = Q[0][1] + Q[1][0]; // a
  out_row[1] = Q[0][2] + Q[2][0]; // b
  out_row[2] = Q[1][2] + Q[2][1]; // c
  out_row[3] = Q[1][3] + Q[3][1]; // d
  out_row[4] = Q[2][3] + Q[3][2]; // e
}
