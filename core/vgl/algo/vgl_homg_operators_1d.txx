// This is core/vgl/algo/vgl_homg_operators_1d.txx
#ifndef vgl_homg_operators_1d_txx_
#define vgl_homg_operators_1d_txx_
//:
// \file

#include "vgl_homg_operators_1d.h"
#include <vnl/vnl_matrix_fixed.h>
#include <vgl/vgl_homg.h> // for infinity
#include <vcl_cmath.h>
#include <vcl_iostream.h>

//-----------------------------------------------------------------------------
//: Calculates the crossratio of four collinear points p1, p2, p3 and p4.
// This number is projectively invariant, and it is the coordinate of p4
// in the reference frame where p2 is the origin (coordinate 0), p3 is
// the unity (coordinate 1) and p1 is the point at infinity.
// This cross ratio is often denoted as ((p1, p2; p3, p4)) (which also
// equals ((p3, p4; p1, p2)) or ((p2, p1; p4, p3)) or ((p4, p3; p2, p1)) )
// and is calculated as
// \verbatim
//                      p1 - p3   p2 - p3      (p1-p3)(p2-p4)
//                      ------- : --------  =  --------------
//                      p1 - p4   p2 - p4      (p1-p4)(p2-p3)
// \endverbatim
//
// In principle, any single nonhomogeneous coordinate from the four points
// can be used as parameters for cross_ratio (but of course the same for all
// points). The most reliable answer will be obtained when the coordinate with
// the largest spacing is used, i.e., the one with smallest slope.
template <class T>
double vgl_homg_operators_1d<T>::cross_ratio(const vgl_homg_point_1d<T>& a,
                                             const vgl_homg_point_1d<T>& b,
                                             const vgl_homg_point_1d<T>& c,
                                             const vgl_homg_point_1d<T>& d)
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  T x3 = c.x(), w3 = c.w();
  T x4 = d.x(), w4 = d.w();
  T n = (x1*w3-x3*w1)*(x2*w4-x4*w2);
  T m = (x1*w4-x4*w1)*(x2*w3-x3*w2);
  if (n == 0 && m == 0)
    vcl_cerr << "cross_ratio not defined: three of the given points coincide\n";
  return n/m;
}

//-----------------------------------------------------------------------------
//: Cross product of two vgl_homg_point_1ds
template <class T>
T vgl_homg_operators_1d<T>::cross(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b)
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  return x1*w2-w1*x2;
}

//-----------------------------------------------------------------------------
//: Dot product of two vgl_homg_point_1ds
template <class T>
T vgl_homg_operators_1d<T>::dot(const vgl_homg_point_1d<T>& a, const vgl_homg_point_1d<T>& b)
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  return x1*x2 + w1*w2;
}

//-----------------------------------------------------------------------------
//: Normalize vgl_homg_point_1d to unit magnitude
template <class T>
void vgl_homg_operators_1d<T>::unitize(vgl_homg_point_1d<T>& a)
{
  double norm = vcl_sqrt (a.x()*a.x() + a.w()*a.w());
  if (norm == 0.0) {
    vcl_cerr << "vgl_homg_operators_1d<T>::unitize() -- Zero length vector\n";
    return;
  }
  norm = 1.0/norm;
  a.set(T(a.x()*norm), T(a.w()*norm));
}

//: Get the distance between the two points.
template <class T>
T vgl_homg_operators_1d<T>::distance (const vgl_homg_point_1d<T>& a,
                                      const vgl_homg_point_1d<T>& b)
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  if (w1 == 0 || w2 == 0) {
    vcl_cerr << "vgl_homg_operators_1d<T>::distance() -- point at infinity";
    return vgl_homg<T>::infinity;
  }
  x1 /= w1; x2 /= w2;
  return (x1 > x2) ? x1-x2 : x2-x1;
}

//: Get the square of the distance between the two points.
template <class T>
T vgl_homg_operators_1d<T>::distance_squared (const vgl_homg_point_1d<T>& point1,
                                              const vgl_homg_point_1d<T>& point2)
{
  T d = distance(point1,point2);
  return d*d;
}

//: Return the midpoint of two homogeneous points
template <class T>
vgl_homg_point_1d<T> vgl_homg_operators_1d<T>::midpoint(const vgl_homg_point_1d<T>& a,
                                                        const vgl_homg_point_1d<T>& b)
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  return vgl_homg_point_1d<T>(x1*w2+x2*w1, 2*w1*w2);
}

//: Calculate the projective conjugate point.
// Or more generally, the point with a given crossratio w.r.t. three other points:
// The cross ratio ((x1,x2;x3,answer)) is cr (default -1). When cr is -1,
// the returned value and x3 are conjugate points w.r.t. the pair (x1,x2).
// Because this function is transitive on coordinates, it is sufficient to
// implement it for 1-dimensional points.
//
template <class T>
T vgl_homg_operators_1d<T>::conjugate(T x1, T x2, T x3, double cr)
// Default for cr is -1.
{
  T a = x1 - x3;  T b = x2 - x3; T c = T(a-cr*b);
  if (c == 0) return (x2*a == cr*x1*b) ? 1 : vgl_homg<T>::infinity;
  return T((x2*a-cr*x1*b)/c);
}

template <class T>
vgl_homg_point_1d<T> vgl_homg_operators_1d<T>::conjugate(const vgl_homg_point_1d<T>& a,
                                                         const vgl_homg_point_1d<T>& b,
                                                         const vgl_homg_point_1d<T>& c,
                                                         double cr)
// Default for cr is -1.
{
  T x1 = a.x(), w1 = a.w();
  T x2 = b.x(), w2 = b.w();
  T x3 = c.x(), w3 = c.w();
  T k = x1*w3 - x3*w1, m = x2*w3 - x3*w2;
  return vgl_homg_point_1d<T>(T(x2*k-cr*x1*m), T(k*w2-cr*m*w1));
  // could be (0,0) !!  not checked.
}

//: Transform a point through a 2x2 projective transformation matrix
// \relates vgl_homg_point_1d
template <class T>
vgl_homg_point_1d<T> operator*(vnl_matrix_fixed<T,2,2> const& m,
                               vgl_homg_point_1d<T> const& p)
{
  return vgl_homg_point_1d<T>(m(0,0)*p.x()+m(0,1)*p.w(),
                              m(1,0)*p.x()+m(1,1)*p.w());
}

#undef VGL_HOMG_OPERATORS_1D_INSTANTIATE
#define VGL_HOMG_OPERATORS_1D_INSTANTIATE(T) \
template class vgl_homg_operators_1d<T >; \
template vgl_homg_point_1d<T > operator*(vnl_matrix_fixed<T,2,2> const& m, vgl_homg_point_1d<T > const& p)

#endif // vgl_homg_operators_1d_txx_
