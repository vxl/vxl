#ifndef boxm_quad_scan_iterator_h_
#define boxm_quad_scan_iterator_h_
//:
// \file
#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_region_scan_iterator.h>
#include <vgl/vgl_polygon_scan_iterator.h>

#include <vbl/vbl_bounding_box.h>

class boxm_quad_scan_iterator : public vgl_region_scan_iterator
{
 public:
  //: constructor
  boxm_quad_scan_iterator(double *verts_x, double *verts_y, unsigned int v0 = 0, unsigned int v1 = 1, unsigned int v2 = 2, unsigned int v3 = 3);
  ~boxm_quad_scan_iterator() override;

  //: Resets the scan iterator to before the first scan line
  //  After calling this function, next() needs to be called before
  //  startx() and endx() form a valid scan line.
  void reset() override;

  //: Tries to move to the next scan line.
  //  Returns false if there are no more scan lines.
  bool next() override;

  //: y-coordinate of the current scan line.
  int  scany() const override;

  //: Returns starting x-value of the current scan line.
  //  startx() should be smaller than endx(), unless the scan line is empty
  int  startx() const override;

  //: Returns ending x-value of the current scan line.
  //  endx() should be larger than startx(), unless the scan line is empty
  int  endx() const override;

  //: returns the amount of pixel at location x in the current scanline covered by the triangle
  float pix_coverage(int x);

  bool x_start_end_val(const double * vals,double & start_val,double & end_val);


 protected:
  static const unsigned int supersample_ratio_ = 1;
  vgl_polygon_scan_iterator<double> * super_it_;
  vgl_polygon<double>  poly_;

  int startx_;
  int endx_;
  int scany_;
  bool next_return_;

  unsigned int poly_diameter_x_;
  float* aa_vals_;
  int aa_vals_offset_;

  vbl_bounding_box<double,2> poly_bb_;
};

#endif
