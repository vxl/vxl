// This is vxl/vgl/vgl_window_scan_iterator.h
#ifndef vgl_window_scan_iterator_h_
#define vgl_window_scan_iterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Iterator to scan rectangular windows
// \author fsm

#include <vcl_cmath.h>
#include <vgl/vgl_region_scan_iterator.h>

//: Iterator to scan rectangular windows
struct vgl_window_scan_iterator : public vgl_region_scan_iterator
{
  int x1, y1, x2, y2;

  //: Define rectangular region to be [x1, x2] x [y1, y2]
  inline void set_window_corners(float x1_, float y1_, float x2_, float y2_) {
    x1 = (int) vcl_ceil (x1_);
    y1 = (int) vcl_ceil (y1_);
    x2 = (int) vcl_floor(x2_);
    y2 = (int) vcl_floor(y2_);
  }

  //: Define rectangular region to be [x-w, x+w] x [y-h, y+h]
  inline void set_window(float x, float y, float w, float h)
    { set_window_corners(x - w, y - h, x + w, y + h); }

  //: Define rectangular region to be [x-r, x+r] x [y-r, y+r]
  inline void set_window(float x, float y, float r)
    { set_window_corners(x - r, y - r, x + r, y + r); }


  //: makes uninitialized iterator.
  inline vgl_window_scan_iterator() { }

  //: assumes x1<=x2, y1<=y2, so region is [x1, x2] x [y1, y2].
  inline vgl_window_scan_iterator(float x1_, float y1_, float x2_, float y2_)
    { set_window_corners(x1_, y1_, x2_, y2_); }

  int current_y;

  inline void reset() { current_y = y1-1; }
  inline bool next () { return (++current_y) <= y2; }
  inline int  scany () const { return current_y; }
  inline int  startx() const { return x1; }
  inline int  endx  () const { return x2; }
};

#endif // vgl_window_scan_iterator_h_
