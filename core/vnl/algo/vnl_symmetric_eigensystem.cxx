// This is core/vnl/algo/vnl_symmetric_eigensystem.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// Created: 29 Aug 96
//
//-----------------------------------------------------------------------------

#include "vnl_symmetric_eigensystem.h"
#include <vcl_cassert.h>
#include <vcl_algorithm.h> // for swap
#include <vcl_cmath.h> // for sqrt(double), acos, etc.
#include <vcl_iostream.h>
#include <vnl/vnl_copy.h>
#include <vnl/algo/vnl_netlib.h> // rs_()



//: Find eigenvalues of a symmetric 3x3 matrix
// \verbatim
// Matrix is   M11  M12  M13
//             M12  M22  M23
//             M13  M23  M33
// \endverbatim
void vnl_symmetric_eigensystem_compute_eigenvals(
  double M11, double M12, double M13,
              double M22, double M23,
                          double M33,
  double &l1, double &l2, double &l3)
{
  const double third = 0.333333333333333333333;
  const double sqrt_three = 1.73205080756887729352;
// Caharacteristic eqtn |M - xI| = 0
// x^3 + b x^2 + c x + d = 0 
  const double b = -M11-M22-M33;
  const double c =  M11*M22 +M11*M33 +M22*M33  -M12*M12 -M13*M13 -M23*M23;
  const double d = M11*M23*M23 +M12*M12*M33 +M13*M13*M22 -2.0*M12*M13*M23 -M11*M22*M33;

// Using the real cubic sover on http://www.1728.com/cubic2.htm
  const double f = c - b*b*third;
  const double g = 0.07407407407407407407407*b*b*b - third*b*c + d;
  const double h = 0.25*g*g + 0.03703703703703703703*f*f*f;
  assert (h <= 0.0);

  if (h==0)
  {
    l1 = l2 = l3 = vcl_pow(-d,third);
    return;
  }

  const double i = vcl_sqrt((0.25*g*g) - h);
  const double j = vcl_pow(i, third);
  const double k = vcl_acos (- (g / (2.0*i)));
  const double m = vcl_cos (third * k);
  const double n = sqrt_three * vcl_sin(third * k);
  const double p = -(third * b);
  l1 = 2.0*j*m + p;
  l2 = -j * (m + n) + p;
  l3 = -j * (m - n) + p;

  if (l2 < l1) vcl_swap(l2, l1);
  if (l3 < l2)
  {
    vcl_swap(l2, l3);
    if (l2 < l1) vcl_swap(l2, l1);
  }
}



bool vnl_symmetric_eigensystem_compute(vnl_matrix<float> const & A,
                                       vnl_matrix<float>       & V,
                                       vnl_vector<float>       & D)
{
  vnl_matrix<double> Ad(A.rows(), A.cols());
  vnl_matrix<double> Vd(V.rows(), V.cols());
  vnl_vector<double> Dd(D.size());
  vnl_copy(A, Ad);
  bool f = vnl_symmetric_eigensystem_compute(Ad, Vd, Dd);
  vnl_copy(Vd, V);
  vnl_copy(Dd, D);
  return f;
}

bool vnl_symmetric_eigensystem_compute(vnl_matrix<double> const & A,
                                       vnl_matrix<double>       & V,
                                       vnl_vector<double>       & D)
{
  A.assert_finite();

  // The fortran code does not like it if V or D are
  // undersized. I expect they probably should not be
  // oversized either. - IMS
  assert(V.rows() == A.rows());
  assert(V.cols() == A.rows());
  assert(D.size() == A.rows());

  int n = A.rows();
  vnl_vector<double> work1(n);
  vnl_vector<double> work2(n);
  vnl_vector<double> Vvec(n*n);

  int want_eigenvectors = 1;
  int ierr = 0;

  // No need to transpose A, cos it's symmetric...
  vnl_matrix<double> B = A; // since A is read-only and rs_ might change its third argument...
  rs_(&n, &n, B.data_block(), &D[0], &want_eigenvectors, &Vvec[0], &work1[0], &work2[0], &ierr);

  if (ierr) {
    vcl_cerr << "vnl_symmetric_eigensystem: ierr = " << ierr << vcl_endl;
    return false;
  }

  // Transpose-copy into V
  double *vptr = &Vvec[0];
  for (int c = 0; c < n; ++c)
    for (int r = 0; r < n; ++r)
      V(r,c) = *vptr++;

  return true;
}

//----------------------------------------------------------------------

// - @{ Solve real symmetric eigensystem $A x = \lambda x$ @}
template <class T>
vnl_symmetric_eigensystem<T>::vnl_symmetric_eigensystem(vnl_matrix<T> const& A)
  : n_(A.rows()), V(n_, n_), D(n_)
{
  vnl_vector<T> Dvec(n_);

  vnl_symmetric_eigensystem_compute(A, V, Dvec);

  // Copy Dvec into diagonal of D
  for (int i = 0; i < n_; ++i)
    D(i,i) = Dvec[i];
}

template <class T>
vnl_vector<T> vnl_symmetric_eigensystem<T>::get_eigenvector(int i) const
{
  return vnl_vector<T>(V.extract(n_,1,0,i).data_block(), n_);
}

template <class T>
T vnl_symmetric_eigensystem<T>::get_eigenvalue(int i) const
{
  return D(i, i);
}

template <class T>
vnl_vector<T> vnl_symmetric_eigensystem<T>::solve(vnl_vector<T> const& b)
{
  //vnl_vector<T> ret(b.length());
  //FastOps::AtB(V, b, &ret);
  vnl_vector<T> ret(b*V); // same as V.transpose()*b

  vnl_vector<T> tmp(b.size());
  D.solve(ret, &tmp);

  return V * tmp;
}

template <class T>
T vnl_symmetric_eigensystem<T>::determinant() const
{
  int const n = D.size();
  T det(1);
  for (int i=0; i<n; ++i)
    det *= D[i];
  return det;
}

template <class T>
vnl_matrix<T> vnl_symmetric_eigensystem<T>::pinverse() const
{
  unsigned n = D.rows();
  vnl_diag_matrix<T> invD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) == 0) {
      vcl_cerr << __FILE__ ": pinverse(): eigenvalue " << i << " is zero.\n";
      invD(i, i) = 0;
    }
    else
      invD(i, i) = 1 / D(i, i);
  return V * invD * V.transpose();
}

template <class T>
vnl_matrix<T> vnl_symmetric_eigensystem<T>::square_root() const
{
  unsigned n = D.rows();
  vnl_diag_matrix<T> sqrtD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) < 0) {
      vcl_cerr << __FILE__ ": square_root(): eigenvalue " << i << " is negative (" << D(i, i) << ").\n";
      sqrtD(i, i) = (T)vcl_sqrt((typename vnl_numeric_traits<T>::real_t)(-D(i, i)));
                    // gives square root of the absolute value of T.
    }
    else
      sqrtD(i, i) = (T)vcl_sqrt((typename vnl_numeric_traits<T>::real_t)(D(i, i)));
  return V * sqrtD * V.transpose();
}

template <class T>
vnl_matrix<T> vnl_symmetric_eigensystem<T>::inverse_square_root() const
{
  unsigned n = D.rows();
  vnl_diag_matrix<T> inv_sqrtD(n);
  for (unsigned i=0; i<n; ++i)
    if (D(i, i) <= 0) {
      vcl_cerr << __FILE__ ": square_root(): eigenvalue " << i << " is non-positive (" << D(i, i) << ").\n";
      inv_sqrtD(i, i) = (T)vcl_sqrt(-1.0/(typename vnl_numeric_traits<T>::real_t)(D(i, i))); // ??
    }
    else
      inv_sqrtD(i, i) = (T)vcl_sqrt(1.0/(typename vnl_numeric_traits<T>::real_t)(D(i, i)));
  return V * inv_sqrtD * V.transpose();
}

//--------------------------------------------------------------------------------

template class vnl_symmetric_eigensystem<float>;
template class vnl_symmetric_eigensystem<double>;
