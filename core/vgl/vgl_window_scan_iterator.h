#ifndef vgl_window_scan_iterator_h_
#define vgl_window_scan_iterator_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_scan_iterator
// .INCLUDE vgl/vgl_scan_iterator.h
// .FILE vgl_scan_iterator.cxx
// .SECTION Author
//  fsm@robots.ox.ac.uk
//

#include <vcl/vcl_cmath.h>
#include <vgl/vgl_region_scan_iterator.h>

struct vgl_window_scan_iterator : vgl_region_scan_iterator
{
  int x1, y1, x2, y2;

  // Assumes x1<=x2, y1<=y2, so region is [x1, x2] x [y1, y2].
  vgl_window_scan_iterator(float x1_, float y1_, float x2_, float y2_)
    : x1((int) ceil (x1_))
    , y1((int) ceil (y1_))
    , x2((int) floor(x2_))
    , y2((int) floor(y2_)) { }

  ~vgl_window_scan_iterator() { }

  int current_y;

  void reset() { current_y = y1-1; }
  bool next() { return (++current_y) <= y2; }
  int  scany() const { return current_y; }
  int  startx() const { return x1; }
  int  endx() const { return x2; }
};

#endif // vgl_window_scan_iterator_h_
