// This is vxl/vgl/vgl_lineseg_test.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgl_polygon_test.h"
#include <vgl/vgl_triangle_test.h>

template <class T>
bool vgl_lineseg_test(T x1, T y1, T x2, T y2,
        T x3, T y3, T x4, T y4)
{
  T a = vgl_triangle_test_discriminant(x1, y1, x2, y2, x3, y3);
  T b = vgl_triangle_test_discriminant(x1, y1, x2, y2, x4, y4);
  return (a<=0 && b>=0) || (a>=0 && b<=0);
}

//--------------------------------------------------------------------------------

#define inst(T) \
template bool vgl_lineseg_test(T, T, T, T, T, T, T, T);
inst(float);
inst(double);
