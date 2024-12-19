// This is core/vpgl/vpgl_affine_fundamental_matrix.hxx
#ifndef vpgl_affine_fundamental_matrix_hxx_
#define vpgl_affine_fundamental_matrix_hxx_
//:
// \file

#include "vpgl_affine_fundamental_matrix.h"
//
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_cross.h>
#include <vnl/vnl_det.h>
#include <vnl/vnl_inverse.h>
#include <vgl/vgl_tolerance.h> // for degenerate determinate
//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix()
  : vpgl_fundamental_matrix<T>()
{
  T m[] = { 0, 0, 0, 0, 0, 1, 0, 1, 0 };
  vnl_matrix_fixed<T, 3, 3> M(m);
  set_matrix(M);
}

//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix(const vnl_matrix_fixed<T, 3, 3> & F)
{
  set_matrix(F);
}

//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix(const vpgl_fundamental_matrix<T> & fm)
{
  set_matrix(fm.get_matrix());
}

//: Convert matrix to double (needed in constructor for float case)
inline vnl_matrix_fixed<double, 3, 4>
vpgl_to_double(const vnl_matrix_fixed<double, 3, 4> & M)
{
  return M;
}

//: Convert matrix to double (needed in constructor for float case)
inline vnl_matrix_fixed<double, 3, 4>
vpgl_to_double(const vnl_matrix_fixed<float, 3, 4> & Mf)
{
  vnl_matrix_fixed<double, 3, 4> M;
  for (unsigned i = 0; i < 12; ++i)
    M.data_block()[i] = double(Mf.data_block()[i]);
  return M;
}

//---------------------------------
template <class T>
vpgl_affine_fundamental_matrix<T>::vpgl_affine_fundamental_matrix(const vpgl_affine_camera<T> & Ar,
                                                                  const vpgl_affine_camera<T> & Al)
{

  // note H&Z use prime ' to indicate quantities in the Al cam.
  // that is x'. ( F x)  = 0 .  So here p == '
  vnl_matrix_fixed<double, 3, 4> M = vpgl_to_double(Ar.get_matrix());
  vnl_matrix_fixed<double, 3, 4> Mp = vpgl_to_double(Al.get_matrix());

  //
  // M = |a00 a01 a02 a03| = |A0 a03|
  //     |a10 a11 a12 a13|   |A1 a13|
  //     | 0   0   0   1 |   |3x0 1 |

  // 1) camera ray  r = A0 X A1
  vnl_vector_fixed<double, 3> A0, A1, r;
  A0[0] = M[0][0];
  A0[1] = M[0][1];
  A0[2] = M[0][2];
  A1[0] = M[1][0];
  A1[1] = M[1][1];
  A1[2] = M[1][2];
  r = vnl_cross_3d(A0, A1);

  //
  // 2) D = |A0.A0  A0.A1|
  //        |A0.A1  A1.A1|
  //
  vnl_matrix_fixed<double, 2, 2> D, Dinv;
  D[0][0] = dot_product(A0, A0);
  D[0][1] = dot_product(A0, A1);
  D[1][0] = D[0][1];
  D[1][1] = dot_product(A1, A1);
  double det = vnl_det(D);
  // det is 2nd order
  if (fabs(det) < T(2) * vgl_tolerance<T>::position)
  {
    std::cout << "Affine fundamental matrix - singular determinant" << std::endl;
    return;
  }
  Dinv = vnl_inverse(D);

  // 3) A'0, A'1 terms p == '
  vnl_vector_fixed<double, 3> Ap0, Ap1;
  Ap0[0] = Mp[0][0];
  Ap0[1] = Mp[0][1];
  Ap0[2] = Mp[0][2];
  Ap1[0] = Mp[1][0];
  Ap1[1] = Mp[1][1];
  Ap1[2] = Mp[1][2];
  vnl_matrix_fixed<double, 2, 2> Dp, DpD;
  Dp[0][0] = dot_product(Ap0, A0);
  Dp[0][1] = dot_product(Ap0, A1);
  Dp[1][0] = dot_product(Ap1, A0);
  Dp[1][1] = dot_product(Ap1, A1);
  DpD = Dp * Dinv;

  // 4)
  //       |0  0  a|
  //  F =  |0  0  b|
  //       |c  d  e|
  //
  double a = dot_product(r, Ap1), b = -dot_product(r, Ap0), c, d, e;

  // 5) k and k'
  vnl_vector_fixed<double, 2> kp, k, t, tp;
  kp[0] = -a;
  kp[1] = -b;
  k = kp * DpD;

  // 6) c, d, e
  c = k[0];
  d = k[1];
  t[0] = M[0][3];
  t[1] = M[1][3];
  tp[0] = Mp[0][3];
  tp[1] = Mp[1][3];
  e = dot_product(kp, tp) - dot_product(k, t);

  // normalization factor
  double norm = 1.0 / std::sqrt(a * a + b * b);

  // set matrix from normalized parameters
  set_from_params(a * norm, b * norm, c * norm, d * norm, e * norm);
}


//---------------------------------
template <class T>
void
vpgl_affine_fundamental_matrix<T>::set_matrix(const vnl_matrix_fixed<T, 3, 3> & F)
{
  set_from_params(F[0][2], F[1][2], F[2][0], F[2][1], F[2][2]);
}


//---------------------------------
template <class T>
void
vpgl_affine_fundamental_matrix<T>::set_from_params(T a, T b, T c, T d, T e)
{
  // note JLM changed to H&Z convention 11/12/2018
  vnl_matrix_fixed<T, 3, 3> F((T)0);
  F.put(0, 2, a);
  F.put(1, 2, b);
  F.put(2, 0, c);
  F.put(2, 1, d);
  F.put(2, 2, e);
  vpgl_fundamental_matrix<T>::set_matrix(F);
};


// Code for easy instantiation.
#undef vpgl_AFFINE_FUNDAMENTAL_MATRIX_INSTANTIATE
#define vpgl_AFFINE_FUNDAMENTAL_MATRIX_INSTANTIATE(T) template class vpgl_affine_fundamental_matrix<T>

#endif // vpgl_affine_fundamental_matrix_hxx_
