// This is core/vgl/vgl_lineseg_test.txx
#ifndef vgl_lineseg_test_txx_
#define vgl_lineseg_test_txx_
//:
// \file
// \author fsm

#include "vgl_lineseg_test.h"
#include <vgl/vgl_triangle_test.h>
#include <vcl_cmath.h>

template <class T>
bool vgl_lineseg_test_line(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4)
{
  T a = vgl_triangle_test_discriminant(x1, y1, x2, y2, x3, y3);
  T b = vgl_triangle_test_discriminant(x1, y1, x2, y2, x4, y4);
  // this condition just says that [3] and [4] lie on opposite
  // sides of the line joining [1], [2].
  return (a<=0 && b>=0) || (a>=0 && b<=0);
}

// Returns true iif (x3,y3) lies inbetween (x1,y1) and (x2,y2);
// all three points are assumed to be collinear
template <class T>
static inline
bool inbetween(T x1, T y1, T x2, T y2, T x3, T y3)
{
  return (x1-x3)*(x2-x3)<=0 && (y1-y3)*(y2-y3)<=0;
}


template <class T>
bool vgl_lineseg_test_lineseg(T x1, T y1, T x2, T y2, T x3, T y3, T x4, T y4)
{
  // reduce precision of inputs so that signs of vgl_triangle_test_discriminants
  // `a', `b', `c', and `d' are more stable when they are very close to zero.

  double px1 = x1;
  double py1 = y1; 
  double px2 = x2;
  double py2 = y2;
  double px3 = x3;
  double py3 = y3;
  double px4 = x4;
  double py4 = y4;

  px1 = (px1 + px1*1e4) - px1*1e4;
  py1 = (py1 + py1*1e4) - py1*1e4;
  px2 = (px2 + px2*1e4) - px2*1e4;
  py2 = (py2 + py2*1e4) - py2*1e4;
  px3 = (px3 + px3*1e4) - px3*1e4;
  py3 = (py3 + py3*1e4) - py3*1e4;
  px4 = (px4 + px4*1e4) - px4*1e4;
  py4 = (py4 + py4*1e4) - py4*1e4;

  // two lines intersect if p1 and p2 are on two opposite sides of the line p3-p4
  // and p3 and p4 are on two opposite sides of line p1-p2
  // degenerate cases (collinear points) are tricky to handle

  double a = vgl_triangle_test_discriminant(px1, py1, px2, py2, px3, py3);
  double b = vgl_triangle_test_discriminant(px1, py1, px2, py2, px4, py4);
  double c = vgl_triangle_test_discriminant(px3, py3, px4, py4, px1, py1);
  double d = vgl_triangle_test_discriminant(px3, py3, px4, py4, px2, py2);

  // force to be zero when they're close to zero
  a = (vcl_abs(a) < 1e-12) ? 0 : a;
  b = (vcl_abs(b) < 1e-12) ? 0 : b;
  c = (vcl_abs(c) < 1e-12) ? 0 : c;
  d = (vcl_abs(d) < 1e-12) ? 0 : d;

  return
    ( ( (a<=0 && b>0) || (a>=0 && b<0) || (a<0 && b>=0) || (a>0 && b<=0) ) &&
      ( (c<=0 && d>0) || (c>=0 && d<0) || (c<0 && d>=0) || (c>0 && d<=0) ) )
    ||
    ( // the above two conditions are only sufficient for noncollinear line segments! - PVr
      a == 0 && b == 0 && c == 0 && d == 0 &&
      ( inbetween(px1, py1, px2, py2, px3, py3) ||
        inbetween(px1, py1, px2, py2, px4, py4) ||
        inbetween(px3, py3, px4, py4, px1, py1) ||
        inbetween(px3, py3, px4, py4, px2, py2) )
    );
}

//--------------------------------------------------------------------------------

#undef VGL_LINESEG_TEST_INSTANTIATE
#define VGL_LINESEG_TEST_INSTANTIATE(T) \
template bool vgl_lineseg_test_line(T, T, T, T, T, T, T, T); \
template bool vgl_lineseg_test_lineseg(T, T, T, T, T, T, T, T)

#endif // vgl_lineseg_test_txx_
