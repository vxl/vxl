#ifndef vgl_region_scan_iterator_h_
#define vgl_region_scan_iterator_h_
#ifdef __GNUC__
#pragma interface
#endif
// This is vxl/vgl/vgl_region_scan_iterator.h

//:
// \file
// \author fsm@robots.ox.ac.uk

//: Abstract base class for iterating over the pixels in a region of image.
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

  //: y-coordinate of the current scan line
  virtual int  scany() const =0;

  //: Returns starting x-value of the current scan line.
  virtual int  startx() const =0;

  //: Returns starting x-value of the current scan line.
  virtual int  endx() const =0;
};

#endif // vgl_region_scan_iterator_h_
