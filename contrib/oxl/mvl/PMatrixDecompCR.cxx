// This is oxl/mvl/PMatrixDecompCR.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
//  \file

#include "PMatrixDecompCR.h"


#include <vcl_iostream.h>

#include <vnl/vnl_matlab_print.h>
#include <vnl/vnl_double_3.h>
#include <vnl/algo/vnl_qr.h>

#include <mvl/PMatrix.h>

PMatrixDecompCR::PMatrixDecompCR(const PMatrix& P, bool scale_C)
{
  compute(P.get_matrix(), scale_C);
}

PMatrixDecompCR::PMatrixDecompCR(const vnl_matrix<double>& P, bool scale_C)
{
  compute(P, scale_C);
}

void UtSolve(const vnl_double_3x3& T, vnl_vector<double>& x)
{
  const double*b = x.data_block();
  x[2] = b[2] / T(2,2);
  x[1] = (b[1] - T(1,2)*x[2])/T(1,1);
  x[0] = (b[0] - T(0,1)*x[1] - T(0,2)*x[2])/T(0,0);
}

//: Decompose P
void PMatrixDecompCR::compute(const vnl_matrix<double>& p, bool scale_C)
{
  // P = [H t]
  //
  vnl_double_3x3 PermHtPerm;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      PermHtPerm(i,j) = p(2-j,2-i);

  vnl_qr<double> qr(PermHtPerm);

  vnl_double_3x3 Q = qr.Q();
  vnl_double_3x3 R = qr.R();

  // Ensure 1st and last diagonal component of C are positive
  // Must insert a det(1) mx between, i.e. two -1s
  bool r0pos = R(0,0) > 0;
  bool r2pos = R(2,2) > 0;
  typedef double d3[3];
  d3 diags[] = {  // 1 2 3
    { -1,  1, -1}, // - x -
    { -1, -1,  1}, // - x +
    {  1, -1, -1}, // + x -
    {  1,  1,  1}, // + x +
  };
  int d = r0pos * 2 + r2pos;
  double* diag = &diags[d][0];

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      C(j,i)  = diag[2-i] * R(2-i,2-j);
      Po(j,i) = diag[2-j] * Q(2-i,2-j);
    }

  // Compute t' = inv(C) t
  vnl_double_3 t;
  for (int i = 0; i < 3; ++i)
    t[i] = p(i,3);
  UtSolve(C, t.as_ref().non_const());
  for (int i = 0; i < 3; ++i)
    Po(i,3) = t[i];

  if (((C * Po - p).fro_norm() > 1e-4) ||
      (C(0,0) < 0) ||
      (C(2,2) < 0)) {
    vcl_cerr << "PMatrixDecompCR: AIEEE!\n";
    MATLABPRINT(p);
    MATLABPRINT((vnl_matrix<double> const&/*2.7*/)C);
    MATLABPRINT((vnl_matrix<double> const&/*2.7*/)Po);
    MATLABPRINT(C * Po - p);
  }

  // Make C(3,3) = 1
  if (scale_C)
    C *= 1.0/C(2,2);
}
