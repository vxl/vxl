#ifndef vgl_polygon_test_h_
#define vgl_polygon_test_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_polygon_test
// .INCLUDE vgl/vgl_polygon_test.h
// .FILE vgl_polygon_test.cxx
// @author fsm@robots.ox.ac.uk

//: return true iff (x, y) is inside (or on boundary of) the given n-gon.
template <class T>
bool vgl_polygon_test_inside(T const *xs, T const *ys, unsigned n, T x, T y);

#endif
