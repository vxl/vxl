#ifndef vgl_polygon_test_h_
#define vgl_polygon_test_h_
#ifdef __GNUC__
#pragma interface "vgl_polygon_test"
#endif
/*
  fsm@robots.ox.ac.uk
*/

//: return true iff (x, y) is inside (or on boundary of) the given n-gon.
template <class T>
bool vgl_polygon_test_inside(T const *xs, T const *ys, unsigned n, T x, T y);

#endif
