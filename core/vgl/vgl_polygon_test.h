// This is vxl/vgl/vgl_polygon_test.h
#ifndef vgl_polygon_test_h_
#define vgl_polygon_test_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

//: return true iff (x, y) is inside (or on boundary of) the given n-gon.
// \relates vgl_polygon
template <class T>
bool vgl_polygon_test_inside(T const *xs, T const *ys, unsigned n, T x, T y);

#endif // vgl_polygon_test_h_
