// This is core/vgl/vgl_region_scan_iterator.h
#ifndef vgl_region_scan_iterator_h_
#define vgl_region_scan_iterator_h_
//:
// \file
// \author fsm
// \verbatim
//  Modifications
//   Nov.2003 - Peter Vanroose - added lots of documentation
// \endverbatim

//: Abstract base class for iterating over the pixels in a region of an image.
//  The region should be "scanline-convex", i.e., every horizontal line should
//  intersect the region in at most one connected part.  Vertically, there may
//  be even disconnected parts: e.g. (part of) a hyperbola with vertical axis.
//  The region should of course be bounded, otherwise iteration makes no sense.
class vgl_region_scan_iterator
{
 protected:
  inline vgl_region_scan_iterator() {}
  virtual ~vgl_region_scan_iterator() {}
 public:

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  virtual void reset() =0;

  //: Tries to move to the next scan line.
  //  Returns false if there are no more scan lines.
  virtual bool next() =0;

  //: y-coordinate of the current scan line.
  //  The next scan line is *not* guaranteed to have scany()+1; use next() instead.
  virtual int  scany() const =0;

  //: Returns starting x-value of the current scan line.
  //  startx() should be smaller than endx(), unless the scan line is empty
  virtual int  startx() const =0;

  //: Returns ending x-value of the current scan line.
  //  endx() should be larger than startx(), unless the scan line is empty
  virtual int  endx() const =0;

  // Utility functions

  //: Number of image points (= integer grid points) inside the region
  inline int count()
  {
    int cnt = 0; reset();
    while (next()) { int n = endx() - startx() + 1; if (n > 0) cnt += n; }
    return cnt;
  }
};

#endif // vgl_region_scan_iterator_h_
