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

  virtual void reset() =0;
  virtual bool next() =0;
  virtual int  scany() const =0;
  virtual int  startx() const =0;
  virtual int  endx() const =0;
};

#endif // vgl_region_scan_iterator_h_
