// This is vxl/vgl/vgl_region_scan_iterator.h
#ifndef vgl_region_scan_iterator_h_
#define vgl_region_scan_iterator_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

//: Abstract base class for iterating over the pixels in a pseudo-convex region of an image.
struct vgl_region_scan_iterator
{
  inline vgl_region_scan_iterator() { }
  virtual ~vgl_region_scan_iterator();

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  virtual void reset() =0;

  //: Tries to moves to the next scan line.
  //  Returns false if there are no more scan lines.
  virtual bool next() =0;

  //: y-coordinate of the current scan line.
  virtual int  scany() const =0;

  //: Returns starting x-value of the current scan line.
  virtual int  startx() const =0;

  //: Returns ending x-value of the current scan line.
  virtual int  endx() const =0;

  // Utility functions
 
  //: Number of image points (= integer grid points) inside the region
  inline int count() {
    int cnt = 0; reset();
    while (next()) { int n = endx() - startx() + 1; if (n > 0) cnt += n; }
    return cnt;
  }
};

#endif // vgl_region_scan_iterator_h_
