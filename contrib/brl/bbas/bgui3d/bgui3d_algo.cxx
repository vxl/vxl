// This is brl/bbas/bgui3d/bgui3d_algo.cxx
#include "bgui3d_algo.h"
//:
// \file

#include <vnl/algo/vnl_qr.h>

//: decompose the camera into internal and external params
bool
bgui3d_decompose_camera( const vnl_double_3x4& camera,
                         vnl_double_3x3& internal_calibration,
                         vnl_double_3x3& rotation,
                         vnl_double_3&   translation )
{
  // camera = [H t]
  //
  vnl_double_3x3 PermH;

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j)
      PermH(i,j) = camera(2-j,2-i);

  vnl_qr<double> qr(PermH.as_ref());

  vnl_double_3x3 Q = qr.Q();
  vnl_double_3x3 R = qr.R();

  // Ensure all diagonal components of C are positive.
  // Must insert a det(+1) or det(-1) mx between.
  int r0pos = R(0,0) > 0 ? 1 : 0;
  int r1pos = R(1,1) > 0 ? 1 : 0;
  int r2pos = R(2,2) > 0 ? 1 : 0;
  typedef double d3[3];
  d3 diags[] = {   // 1 2 3
    { -1, -1, -1}, // - - -
    {  1, -1, -1}, // + - -
    { -1,  1, -1}, // - + -
    {  1,  1, -1}, // + + -
    { -1, -1,  1}, // - - +
    {  1, -1,  1}, // + - +
    { -1,  1,  1}, // - + +
    {  1,  1,  1}, // + + +
  };
  int d = r0pos * 4 + r1pos * 2 + r2pos;
  double* diag = &diags[d][0];

  for (int i = 0; i < 3; ++i)
    for (int j = 0; j < 3; ++j) {
      internal_calibration(j,i) = diag[i] * R(2-i,2-j);
      rotation(j,i) = diag[j] * Q(2-i,2-j);
    }

  // Compute t' = inv(C) t
  vnl_double_3 t;
  for (int i = 0; i < 3; ++i)
    t[i] = camera(i,3);

  t[2] /= internal_calibration(2,2);
  t[1] = (t[1] - internal_calibration(1,2)*t[2])/internal_calibration(1,1);
  t[0] = (t[0] - internal_calibration(0,1)*t[1] - internal_calibration(0,2)*t[2])/internal_calibration(0,0);

  translation = t;

  // Recompose the matrix and compare
  vnl_double_3x4 Po;
  Po.set_columns(0,rotation.as_ref());
  Po.set_column(3,translation);
  if (((internal_calibration * Po - camera).fro_norm() > 1e-4) ||
      (internal_calibration(0,0) < 0) ||
      (internal_calibration(1,1) < 0) ||
      (internal_calibration(2,2) < 0)) {
    return false;
  }

  // Scale the internal calibration matrix such that the bottom right is 1
  internal_calibration /= internal_calibration(2,2);

  return true;
}
