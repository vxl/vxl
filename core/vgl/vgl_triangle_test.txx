// This is core/vgl/vgl_triangle_test.txx
#ifndef vgl_triangle_test_txx_
#define vgl_triangle_test_txx_
//:
// \file
// \author fsm

#include "vgl_triangle_test.h"

template <class T>
T vgl_triangle_test_discriminant(T x1, T y1,
                                 T x2, T y2,
                                 T x3, T y3)
{
  return x1*(y2-y3) - x2*(y1-y3) + x3*(y1-y2);
}

template <class T>
bool vgl_triangle_test_inside(T x1, T y1,
                              T x2, T y2,
                              T x3, T y3,
                              T x , T y )
{
  T det123 = vgl_triangle_test_discriminant(x1, y1,   x2, y2,   x3, y3);
  T detA23 = vgl_triangle_test_discriminant( x,  y,   x2, y2,   x3, y3);
  T det1A3 = vgl_triangle_test_discriminant(x1, y1,    x,  y,   x3, y3);
  T det12A = vgl_triangle_test_discriminant(x1, y1,   x2, y2,    x,  y);

  if (det123 > 0)
    return detA23>=0 && det1A3>=0 && det12A>=0;

  else if (det123 < 0)
    return detA23<=0 && det1A3<=0 && det12A<=0;

  else // det123 == 0 or NaN
    // degenerate triangle. ignore for now.
    return false;
}

//----------------------------------------

#undef VGL_TRIANGLE_TEST_INSTANTIATE
#define VGL_TRIANGLE_TEST_INSTANTIATE(T) \
template T    vgl_triangle_test_discriminant(T, T, T, T, T, T); \
template bool vgl_triangle_test_inside(T, T, T, T, T, T, T, T)

#endif // vgl_triangle_test_txx_
