#ifndef boxm_triangle_interpolation_iterator_h_
#define boxm_triangle_interpolation_iterator_h_
//:
// \file
#include <iostream>
#include <vector>
#include <vcl_compiler.h>

#include <vgl/vgl_region_scan_iterator.h>
#include <vgl/vgl_triangle_scan_iterator.h>

template<class T>
class boxm_triangle_interpolation_iterator : public vgl_region_scan_iterator
{
 public:
  //: constructor
  boxm_triangle_interpolation_iterator(double *verts_x, double *verts_y, T *values, unsigned int v0 = 0, unsigned int v1 = 1, unsigned int v2 = 2);

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  virtual void reset();

  //: Tries to move to the next scan line.
  //  Returns false if there are no more scan lines.
  virtual bool next();

  //: y-coordinate of the current scan line.
  virtual int  scany() const;

  //: Returns starting x-value of the current scan line.
  //  startx() should be smaller than endx(), unless the scan line is empty
  virtual int  startx() const;

  //: Returns ending x-value of the current scan line.
  //  endx() should be larger than startx(), unless the scan line is empty
  virtual int  endx() const;

  //: returns the interpolated value at location x in the current scanline
  T value_at(int x);

 protected:

  vgl_triangle_scan_iterator<double> tri_it_;

  double s0_;
  double s1_;
  double s2_;
};

#endif
