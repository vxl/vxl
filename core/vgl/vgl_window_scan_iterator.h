// This is core/vgl/vgl_window_scan_iterator.h
#ifndef vgl_window_scan_iterator_h_
#define vgl_window_scan_iterator_h_
//:
// \file
// \brief Iterator to scan rectangular windows
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - made class vgl_window_scan_iterator templated
//   Nov.2003 - Peter Vanroose - made constructor more robust w.r.t. its input
// \endverbatim

#include <cmath>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_region_scan_iterator.h>

//: Iterator to scan rectangular windows
//  This class is completely inlined.
template <class T>
class vgl_window_scan_iterator : public vgl_region_scan_iterator
{
  int x1, y1, x2, y2;
 public:

  //: Define rectangular region to be [x1, x2] x [y1, y2]
  inline void set_window_corners(T x1_, T y1_, T x2_, T y2_)
  {
    // Make sure that the scan lines have positive x increment:
    if (x1_ > x2_) { T x=x1_; x1_=x2_; x2_=x; }
    x1 = (int) std::ceil (x1_);
    x2 = (int) std::floor(x2_);
    // subsequent scan lines need not have positive y increment:
    if (y1_ <= y2_) {
      y1 = (int) std::ceil (y1_);
      y2 = (int) std::floor(y2_);
    }
    else {
      y2 = (int) std::ceil (y2_);
      y1 = (int) std::floor(y1_);
    }
  }

  //: Define rectangular region to be [x-w, x+w] x [y-h, y+h]
  inline void set_window(T x, T y, T w, T h)
  { set_window_corners(x - w, y - h, x + w, y + h); }

  //: Define rectangular region to be [x-r, x+r] x [y-r, y+r]
  inline void set_window(T x, T y, T r)
  { set_window_corners(x - r, y - r, x + r, y + r); }

  //: makes uninitialized iterator.
  inline vgl_window_scan_iterator() = default;

  //: region is [x1, x2] x [y1, y2].  No assumption about x1<x2 or y1<y2.
  inline vgl_window_scan_iterator(T x1_, T y1_, T x2_, T y2_)
  { set_window_corners(x1_, y1_, x2_, y2_); }

  int current_y;

  inline void reset() override { current_y = y1<=y2 ? y1-1 : y1+1; }
  inline bool next () override { return y1<=y2 ? ++current_y <= y2 : --current_y >= y2; }
  inline int  scany () const override { return current_y; }
  inline int  startx() const override { return x1; }
  inline int  endx  () const override { return x2; }
};

#define VGL_WINDOW_SCAN_ITERATOR_INSTANTIATE(T) extern "please include <vgl/vgl_window_scan_iterator.hxx> instead"

#endif // vgl_window_scan_iterator_h_
