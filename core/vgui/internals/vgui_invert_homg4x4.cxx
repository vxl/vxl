// This is core/vgui/internals/vgui_invert_homg4x4.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm

#include "vgui_invert_homg4x4.h"

static bool is_diagonal(double const * const *M)
{
  for (unsigned i=0; i<4; ++i)
    for (unsigned j=0; j<4; ++j)
      if ( (i != j) && (M[i][j] != 0) )
        return false;
  return true;
}

// Translation plus scale
static bool is_trans_scale(double const * const *M)
{
  return                  M[0][1] == 0 && M[0][2] == 0 &&
          M[1][0] == 0 &&                 M[1][2] == 0 &&
          M[2][0] == 0 && M[2][1] == 0 &&
          M[3][0] == 0 && M[3][1] == 0 && M[3][2] == 0;
}

// return pointer to static data
static double const * const * detA_inverseA(double const * const *A/*4x4*/)
{
  static double data[4][4]={
    { A[1][1]*A[2][2]*A[3][3]-A[1][1]*A[2][3]*A[3][2]-A[2][1]*A[1][2]*A[3][3]
     +A[2][1]*A[1][3]*A[3][2]+A[3][1]*A[1][2]*A[2][3]-A[3][1]*A[1][3]*A[2][2],
     -A[0][1]*A[2][2]*A[3][3]+A[0][1]*A[2][3]*A[3][2]+A[2][1]*A[0][2]*A[3][3]
     -A[2][1]*A[0][3]*A[3][2]-A[3][1]*A[0][2]*A[2][3]+A[3][1]*A[0][3]*A[2][2],
      A[0][1]*A[1][2]*A[3][3]-A[0][1]*A[1][3]*A[3][2]-A[1][1]*A[0][2]*A[3][3]
     +A[1][1]*A[0][3]*A[3][2]+A[3][1]*A[0][2]*A[1][3]-A[3][1]*A[0][3]*A[1][2],
     -A[0][1]*A[1][2]*A[2][3]+A[0][1]*A[1][3]*A[2][2]+A[1][1]*A[0][2]*A[2][3]
     -A[1][1]*A[0][3]*A[2][2]-A[2][1]*A[0][2]*A[1][3]+A[2][1]*A[0][3]*A[1][2] },
    {-A[1][0]*A[2][2]*A[3][3]+A[1][0]*A[2][3]*A[3][2]+A[2][0]*A[1][2]*A[3][3]
     -A[2][0]*A[1][3]*A[3][2]-A[3][0]*A[1][2]*A[2][3]+A[3][0]*A[1][3]*A[2][2],
      A[0][0]*A[2][2]*A[3][3]-A[0][0]*A[2][3]*A[3][2]-A[2][0]*A[0][2]*A[3][3]
     +A[2][0]*A[0][3]*A[3][2]+A[3][0]*A[0][2]*A[2][3]-A[3][0]*A[0][3]*A[2][2],
     -A[0][0]*A[1][2]*A[3][3]+A[0][0]*A[1][3]*A[3][2]+A[1][0]*A[0][2]*A[3][3]
     -A[1][0]*A[0][3]*A[3][2]-A[3][0]*A[0][2]*A[1][3]+A[3][0]*A[0][3]*A[1][2],
      A[0][0]*A[1][2]*A[2][3]-A[0][0]*A[1][3]*A[2][2]-A[1][0]*A[0][2]*A[2][3]
     +A[1][0]*A[0][3]*A[2][2]+A[2][0]*A[0][2]*A[1][3]-A[2][0]*A[0][3]*A[1][2] },
    { A[1][0]*A[2][1]*A[3][3]-A[1][0]*A[2][3]*A[3][1]-A[2][0]*A[1][1]*A[3][3]
     +A[2][0]*A[1][3]*A[3][1]+A[3][0]*A[1][1]*A[2][3]-A[3][0]*A[1][3]*A[2][1],
     -A[0][0]*A[2][1]*A[3][3]+A[0][0]*A[2][3]*A[3][1]+A[2][0]*A[0][1]*A[3][3]
     -A[2][0]*A[0][3]*A[3][1]-A[3][0]*A[0][1]*A[2][3]+A[3][0]*A[0][3]*A[2][1],
      A[0][0]*A[1][1]*A[3][3]-A[0][0]*A[1][3]*A[3][1]-A[1][0]*A[0][1]*A[3][3]
     +A[1][0]*A[0][3]*A[3][1]+A[3][0]*A[0][1]*A[1][3]-A[3][0]*A[0][3]*A[1][1],
     -A[0][0]*A[1][1]*A[2][3]+A[0][0]*A[1][3]*A[2][1]+A[1][0]*A[0][1]*A[2][3]
     -A[1][0]*A[0][3]*A[2][1]-A[2][0]*A[0][1]*A[1][3]+A[2][0]*A[0][3]*A[1][1] },
    {-A[1][0]*A[2][1]*A[3][2]+A[1][0]*A[2][2]*A[3][1]+A[2][0]*A[1][1]*A[3][2]
     -A[2][0]*A[1][2]*A[3][1]-A[3][0]*A[1][1]*A[2][2]+A[3][0]*A[1][2]*A[2][1],
      A[0][0]*A[2][1]*A[3][2]-A[0][0]*A[2][2]*A[3][1]-A[2][0]*A[0][1]*A[3][2]
     +A[2][0]*A[0][2]*A[3][1]+A[3][0]*A[0][1]*A[2][2]-A[3][0]*A[0][2]*A[2][1],
     -A[0][0]*A[1][1]*A[3][2]+A[0][0]*A[1][2]*A[3][1]+A[1][0]*A[0][1]*A[3][2]
     -A[1][0]*A[0][2]*A[3][1]-A[3][0]*A[0][1]*A[1][2]+A[3][0]*A[0][2]*A[1][1],
      A[0][0]*A[1][1]*A[2][2]-A[0][0]*A[1][2]*A[2][1]-A[1][0]*A[0][1]*A[2][2]
     +A[1][0]*A[0][2]*A[2][1]+A[2][0]*A[0][1]*A[1][2]-A[2][0]*A[0][2]*A[1][1] }
  };
  static double *out[4] = {data[0], data[1], data[2], data[3]};
  return out;
}

bool vgui_invert_homg4x4(double const * const *M, double **Mi)
{
  if (is_diagonal(M)) {
    // * 0 0 0
    // 0 * 0 0
    // 0 0 * 0
    // 0 0 0 *
    // Assume diagonal elements are non-zero.
    for (unsigned i=0; i<4; ++i)
      for (unsigned j=0; j<4; ++j)
        Mi[i][j] = (i==j ? 1.0/M[i][i] : 0);
    return true;
  }

  if (is_trans_scale(M)) {
    // * 0 0 *
    // 0 * 0 *
    // 0 0 * *
    // 0 0 0 *
    Mi[0][0] = 1/M[0][0]; Mi[0][1] = 0;         Mi[0][2] = 0;         Mi[0][3] = -M[0][3]/M[0][0];
    Mi[1][0] = 0;         Mi[1][1] = 1/M[1][1]; Mi[1][2] = 0;         Mi[1][3] = -M[1][3]/M[1][1];
    Mi[2][0] = 0;         Mi[2][1] = 0;         Mi[2][2] = 1/M[2][2]; Mi[2][3] = -M[2][3]/M[2][2];
    Mi[3][0] = 0;         Mi[3][1] = 0;         Mi[3][2] = 0;         Mi[3][3] =        1/M[3][3];
    return true;
  }

  // add more easy cases here....

  // closed-form inversion.
  // may be much faster.
  // may be less accurate.
  // if the given matrix has rank 0,1 or 2, the computed inverse will be zero.
  // if the given matrix has rank 3, the computed inverse will have rank 1.
  // else, the computed matrix should have rank 4.
  double const * const *out = detA_inverseA(M); // = vnl_inverse(M)
  for (unsigned i=0; i<4; ++i)
    for (unsigned j=0; j<4; ++j)
      Mi[i][j] = out[i][j];

  return true;
}

bool vgui_invert_homg4x4(double const A[4][4], double B[4][4])
{
  double const *A_[4] = {A[0], A[1], A[2], A[3]};
  double       *B_[4] = {B[0], B[1], B[2], B[3]};
  return vgui_invert_homg4x4(A_, B_);
}
