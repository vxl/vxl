// This is oxl/mvl/PMatrixDecompCR.cxx
//:
//  \file

#include <iostream>
#include "PMatrixDecompCR.h"


#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vnl/vnl_matlab_print.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_qr.h>

#include <mvl/PMatrix.h>

PMatrixDecompCR::PMatrixDecompCR(const PMatrix& P, bool scale_C)
{
  compute(P.get_matrix(), scale_C);
}

PMatrixDecompCR::PMatrixDecompCR(const vnl_double_3x4& P, bool scale_C)
{
  compute(P, scale_C);
}

void UtSolve(const vnl_double_3x3& T, vnl_double_3& x)
{
  x[2] /= T(2,2);
  x[1] = (x[1] - T(1,2)*x[2])/T(1,1);
  x[0] = (x[0] - T(0,1)*x[1] - T(0,2)*x[2])/T(0,0);
}

//: Decompose P
void PMatrixDecompCR::compute(const vnl_double_3x4& p, bool scale_C)
{
  // P = [H t]
  //
  vnl_double_3x3 PermHtPerm;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      PermHtPerm(i,j) = p(2-j,2-i);

  vnl_qr<double> qr(PermHtPerm.as_ref()); // size 3x3

  vnl_double_3x3 Q = qr.Q();
  vnl_double_3x3 R = qr.R();

  // Ensure all diagonal components of C are positive.
  // Must insert a det(+1) or det(-1) mx between.
  int r0pos = R(0,0) > 0 ? 1 : 0;
  int r1pos = R(1,1) > 0 ? 1 : 0;
  int r2pos = R(2,2) > 0 ? 1 : 0;
  typedef double d3[3];
  d3 diags[] = {   // 1 2 3
    { -1,  1, -1}, // - + -
    {  1, -1, -1}, // - + +
    { -1, -1, -1}, // - - -
    {  1,  1, -1}, // - - +
    { -1, -1,  1}, // + + -
    {  1,  1,  1}, // + + +
    { -1,  1,  1}, // + - -
    {  1, -1,  1}, // + - +
  };
  int d = r0pos * 4 + r1pos * 2 + r2pos;
  double* diag = &diags[d][0];

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      C(j,i)  = diag[i] * R(2-i,2-j);
      Po(j,i) = diag[j] * Q(2-i,2-j);
    }

  // Compute t' = inv(C) t
  vnl_double_3 t;
  for (int i = 0; i < 3; ++i)
    t[i] = p(i,3);
  UtSolve(C, t);

  for (int i = 0; i < 3; ++i)
    Po(i,3) = t[i];

  if (((C * Po - p).fro_norm() > 1e-4) ||
      (C(0,0) < 0) ||
      (C(1,1) < 0) ||
      (C(2,2) < 0)) {
    std::cerr << "PMatrixDecompCR: AIEEE!\n";
    vnl_matlab_print(std::cerr, p, "p");
    vnl_matlab_print(std::cerr, C, "C");
    vnl_matlab_print(std::cerr, Po, "Po");
    vnl_matlab_print(std::cerr, C * Po - p, "C * Po - p");
  }

  // Make C(2,2) = 1
  if (scale_C)
    C *= 1.0/C(2,2);
}
