#ifndef vgl_window_scan_iterator_h_
#define vgl_window_scan_iterator_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_window_scan_iterator
// .INCLUDE vgl/vgl_window_scan_iterator.h
// .FILE vgl_window_scan_iterator.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//

#include <vcl/vcl_cmath.h>
#include <vgl/vgl_region_scan_iterator.h>

struct vgl_window_scan_iterator : vgl_region_scan_iterator
{
  int x1, y1, x2, y2;

  // [x1, x2] x [y1, y2]
  void set_window_corners(float x1_, float y1_, float x2_, float y2_) {
    x1 = (int) ceil (x1_);
    y1 = (int) ceil (y1_);
    x2 = (int) floor(x2_);
    y2 = (int) floor(y2_);
  }

  // [x-w, x+w] x [y-h, y+h]
  void set_window(float x, float y, float w, float h) 
    { set_window_corners(x - w, y - w, x + w, y + w); }

  // [x-r, x+r] x [y-r, y+r]
  void set_window(float x, float y, float r) 
    { set_window_corners(x - r, y - r, x + r, y + r); }


  // makes uninitialized iterator.
  vgl_window_scan_iterator() { }

  // assumes x1<=x2, y1<=y2, so region is [x1, x2] x [y1, y2].
  vgl_window_scan_iterator(float x1_, float y1_, float x2_, float y2_)
    { set_window_corners(x1_, y1_, x2_, y2_); }
  
  int current_y;

  void reset() { current_y = y1-1; }
  bool next () { return (++current_y) <= y2; }
  int  scany () const { return current_y; }
  int  startx() const { return x1; }
  int  endx  () const { return x2; }
};

#endif // vgl_window_scan_iterator_h_
