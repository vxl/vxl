// This is vxl/vgl/vgl_triangle_scan_iterator.h
#ifndef vgl_triangle_scan_iterator_h_
#define vgl_triangle_scan_iterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//: \file
// \brief Optimized polygon scan iterator for triangles
// \author fsm@robots.ox.ac.uk

#include <vgl/vgl_region_scan_iterator.h>

#define use_polygon_scan_iterator 0

//: Optimized polygon scan iterator for triangles.
struct vgl_triangle_scan_iterator : public vgl_region_scan_iterator
{
  //: Vertices of triangle
  struct pt { double x; double y; } a, b, c;

#if use_polygon_scan_iterator
  inline vgl_triangle_scan_iterator() : data(0) { }
  ~vgl_triangle_scan_iterator();
#endif

  void reset();
  bool next();
  inline int  scany() const { return scany_; }
  inline int  startx() const { return startx_; }
  inline int  endx() const { return endx_; }

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
  double data[3][3];
#endif
};

#endif // vgl_triangle_scan_iterator_h_
