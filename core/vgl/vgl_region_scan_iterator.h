#ifndef vgl_region_scan_iterator_h_
#define vgl_region_scan_iterator_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgl_region_scan_iterator
// .INCLUDE vgl/vgl_region_scan_iterator.h
// .FILE vgl_region_scan_iterator.cxx
// @author fsm@robots.ox.ac.uk

// Abstract base class for iterating over the pixels in
// a region of image.

struct vgl_region_scan_iterator
{
  vgl_region_scan_iterator() { }
  virtual ~vgl_region_scan_iterator() { }

  virtual void reset() =0;
  virtual bool next() =0;
  virtual int  scany() const =0;
  virtual int  startx() const =0;
  virtual int  endx() const =0;
};

#endif
