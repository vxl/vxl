// This is core/vgl/vgl_lineseg_test.txx
#ifndef vgl_lineseg_test_txx_
#define vgl_lineseg_test_txx_
//:
// \file
// \author fsm

#include "vgl_lineseg_test.h"
#include <vgl/vgl_triangle_test.h>

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
  T a = vgl_triangle_test_discriminant(x1, y1, x2, y2, x3, y3);
  T b = vgl_triangle_test_discriminant(x1, y1, x2, y2, x4, y4);
  T c = vgl_triangle_test_discriminant(x3, y3, x4, y4, x1, y1);
  T d = vgl_triangle_test_discriminant(x3, y3, x4, y4, x2, y2);
  return
    ( ( (a<=0 && b>0) || (a>=0 && b<0) || (a<0 && b>=0) || (a>0 && b<=0) ) &&
      ( (c<=0 && d>0) || (c>=0 && d<0) || (c<0 && d>=0) || (c>0 && d<=0) ) )
    ||
    ( // the above two conditions are only sufficient for noncollinear line segments! - PVr
      a == 0 && b == 0 && c == 0 && d == 0 &&
      ( inbetween(x1, y1, x2, y2, x3, y3) ||
        inbetween(x1, y1, x2, y2, x4, y4) )
    );
}

//--------------------------------------------------------------------------------

#undef VGL_LINESEG_TEST_INSTANTIATE
#define VGL_LINESEG_TEST_INSTANTIATE(T) \
template bool vgl_lineseg_test_line(T, T, T, T, T, T, T, T); \
template bool vgl_lineseg_test_lineseg(T, T, T, T, T, T, T, T)

#endif // vgl_lineseg_test_txx_
