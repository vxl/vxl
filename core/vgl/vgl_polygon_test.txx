// This is core/vgl/vgl_polygon_test.txx
#ifndef vgl_polygon_test_txx_
#define vgl_polygon_test_txx_
//:
// \file
// \author fsm

#include "vgl_polygon_test.h"
#include <vgl/vgl_lineseg_test.h>

template <class T>
bool vgl_polygon_test_inside(T const *xs, T const *ys, unsigned n, T x, T y)
{
  // compute centre
  T cx = 0;
  T cy = 0;
  for (unsigned i=0; i<n; ++i) {
    cx += xs[i];
    cy += ys[i];
  }
  cx /= n;
  cy /= n;

  // compute a point outside the polygon.
  T ox = 0, oy = 0;
  for (unsigned i=0; i<n; ++i) {
    T tmp;

    tmp = xs[i]-cx;
    if (tmp<0) tmp = -tmp;
    if (tmp>ox) ox = tmp;

    tmp = ys[i]-cy;
    if (tmp<0) tmp = -tmp;
    if (tmp>oy) oy = tmp;
  }
  ox = cx + ox + oy + 1;
  oy = cy + ox + oy + 1;

  // count crossings.
  unsigned crossings = 0;
  for (unsigned i=0; i<n; ++i)
    if (vgl_lineseg_test_lineseg(xs[i], ys[i], xs[(i+1)%n], ys[(i+1)%n],   ox, oy, x, y))
      ++crossings;

  // inside iff there was an odd number of crossings.
  return crossings % 2 != 0;
}

//--------------------------------------------------------------------------------

#undef VGL_POLYGON_TEST_INSTANTIATE
#define VGL_POLYGON_TEST_INSTANTIATE(T) \
template bool vgl_polygon_test_inside(T const*, T const*, unsigned, T, T)

#endif // vgl_polygon_test_txx_
