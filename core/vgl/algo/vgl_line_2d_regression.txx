// This is core/vgl/algo/vgl_line_2d_regression.txx
#ifndef vgl_line_2d_regression_txx_
#define vgl_line_2d_regression_txx_
//:
// \file

#include "vgl_line_2d_regression.h"
#include <vcl_cmath.h>
#include <vgl/vgl_distance.h>
#include <vnl/vnl_matrix_fixed.h>
#include <vnl/algo/vnl_symmetric_eigensystem.h>
#include <vcl_iostream.h>
#include <vcl_cassert.h>

//: Constructor
template <class T>
vgl_line_2d_regression<T>::vgl_line_2d_regression()
{
  this->clear();
}

//: Add a point to the current regression sums
template <class T>
void vgl_line_2d_regression<T>::increment_partial_sums(const T x, const T y)
{
  Sx_ += x;
  Sy_ += y;
  Sxx_ += x*x;
  Sxy_ += x*y;
  Syy_ += y*y;
  ++npts_;
}

//: Remove a point from the current regression sums
template <class T>
void vgl_line_2d_regression<T>::decrement_partial_sums(const T x, const T y)
{
  assert(npts_!=0);
  Sx_ -= x;
  Sy_ -= y;
  Sxx_ -= x*x;
  Sxy_ -= x*y;
  Syy_ -= y*y;
  --npts_;
}

//: Clear the regression sums
template <class T>
void vgl_line_2d_regression<T>::clear()
{
  Sx_ = 0;
  Sy_ = 0;
  Sxx_ = 0;
  Sxy_ = 0;
  Syy_ = 0;
  npts_=0;
  squared_error_=0;
}

//: Fit a line to the current regression data
template <class T>
bool vgl_line_2d_regression<T>::fit()
{
  if (npts_<2)
  {
    vcl_cout << "In vgl_line_2d_regression<T>::fit() - less than 2 points\n";
    return false;
  }
  vnl_matrix_fixed<T, 2, 2> M;
  M(0, 0) = Sxx_-Sx_*Sx_/npts_;
  M(0, 1) = M(1, 0) = Sxy_-Sx_*Sy_/npts_;
  M(1, 1) = Syy_-Sy_*Sy_/npts_;

  vnl_symmetric_eigensystem<T> sym(M);
  T a = sym.V(0,0);
  T b = sym.V(1,0);
  T c = -(a*Sx_/npts_ + b*Sy_/npts_);
  line_= vgl_line_2d<T>(a,b,c);

  return true;
}

template <class T>
bool vgl_line_2d_regression<T>::fit_constrained(T x, T y)
{
  if (npts_<1)
  {
    vcl_cout << "In vgl_line_2d_regression<T>::fit_constrained() - less than 1 point\n";
    return false;
  }
  vnl_matrix_fixed<T, 2, 2> M;
  M(0, 0) = Sxx_-2*Sx_*x +npts_*x*x;
  M(0, 1) = M(1, 0) = Sxy_-Sx_*y-x*Sy_+npts_*x*y;
  M(1, 1) = Syy_-2*Sy_*y+npts_*y*y;
  vnl_symmetric_eigensystem<T> sym(M);
  T a = sym.V(0,0);
  T b = sym.V(1,0);
  T c = -(a*x + b*y);
  line_= vgl_line_2d<T>(a,b,c);
  return true;
}

template <class T>
double vgl_line_2d_regression<T>::get_rms_error(const T a, const T b, const T c)
{
  if (npts_==0)
    return 0;
  double t0 = Sxx_*a*a + 2*Sxy_*a*b + Syy_*b*b;
  double t1 = 2*Sx_*a*c + 2*Sy_*b*c + npts_*c*c;
  double rms = vcl_sqrt(vcl_fabs(t0+t1)/((a*a+b*b)*npts_));
  return rms;
}

template <class T>
double vgl_line_2d_regression<T>::get_rms_error()
{
  return this->get_rms_error(line_.a(), line_.b(), line_.c());
}

//==================================================================
//:
// We want to add points to the regression until it is likely that
// the fitting error has been exceeded.
//
// \verbatim
//  squared_error_  = squared_error_ + d^2
//                   ---------------------
//                         npts_+1
// \endverbatim
//===================================================================
// Initialize the recursive estimation of fitting error
template <class T>
void vgl_line_2d_regression<T>::init_rms_error_est()
{
  squared_error_ = this->get_rms_error();
  squared_error_ *=squared_error_;
  squared_error_ *= npts_;
}

//: estimate of the fitting error if a new point is added.
// Worst case is distance from the point, (x, y) to the current line.
// Add the error to the accumulating estimation sum.
template <class T>
double vgl_line_2d_regression<T>::get_rms_error_est(vgl_point_2d<T> const&  p,
                                                    bool increment)
{
  if (npts_==0)
    return 0;
  double d = vgl_distance(p, line_);
  double ds = d*d;
  if (increment)
    squared_error_ = squared_error_ + ds;
  return vcl_sqrt(squared_error_/(npts_+1));
}

//----------------------------------------------------------------------------
#undef VGL_LINE_2D_REGRESSION_INSTANTIATE
#define VGL_LINE_2D_REGRESSION_INSTANTIATE(T) \
template class vgl_line_2d_regression<T >

#endif // vgl_line_2d_regression_txx_
