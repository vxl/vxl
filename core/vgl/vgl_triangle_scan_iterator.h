// This is core/vgl/vgl_triangle_scan_iterator.h
#ifndef vgl_triangle_scan_iterator_h_
#define vgl_triangle_scan_iterator_h_
//:
// \file
// \brief Optimized polygon scan iterator for triangles
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - made class vgl_triangle_scan_iterator templated
// \endverbatim

#include <vgl/vgl_region_scan_iterator.h>

#define use_polygon_scan_iterator 0

//: Optimized polygon scan iterator for triangles.
template <class T>
class vgl_triangle_scan_iterator : public vgl_region_scan_iterator
{
 public:
  //: Vertices of triangle
  struct pt { T x; T y; } a, b, c;

#if use_polygon_scan_iterator
  vgl_triangle_scan_iterator() : data(0) {}
 ~vgl_triangle_scan_iterator();
#endif

  void reset();
  bool next();
  inline int scany() const { return scany_; }
  inline int startx() const { return startx_; }
  inline int endx() const { return endx_; }

 private:
  int scany_;
  int startx_;
  int endx_;

#if use_polygon_scan_iterator
  struct data_t;
  data_t *data;
#else
  int y0, y1;
  int x0, x1;

  pt g; // centroid, for conditioning
  T data[3][3];
#endif
};

#define VGL_TRIANGLE_SCAN_ITERATOR_INSTANTIATE(T) extern "please include <vgl/vgl_triangle_scan_iterator.txx> instead"

#endif // vgl_triangle_scan_iterator_h_
