// This is core/vgl/vgl_polygon_test.h
#ifndef vgl_polygon_test_h_
#define vgl_polygon_test_h_
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - made function templated
// \endverbatim

//: return true iff (x, y) is inside (or on boundary of) the given n-gon.
// \relates vgl_polygon
template <class T>
bool vgl_polygon_test_inside(T const *xs, T const *ys, unsigned n, T x, T y);

#define VGL_POLYGON_TEST_INSTANTIATE(T) extern "please include vgl/vgl_polygon_test.txx instead"

#endif // vgl_polygon_test_h_
