#include <vcl_vector.h>
#include <vcl_cmath.h>
#include <vcl_algorithm.h>
#include <vcl_iostream.h>

#include "boxm_quad_scan_iterator.h"


//: constructor
boxm_quad_scan_iterator::boxm_quad_scan_iterator(double *verts_x, double *verts_y, 
													   unsigned int v0, unsigned int v1, 
													   unsigned int v2, unsigned int v3)
{
  vgl_polygon<double> poly;
  poly.new_sheet();
  
  
  poly.push_back(verts_x[v0] * supersample_ratio_ - 0.5,
				 verts_y[v0] * supersample_ratio_ - 0.5);
  poly.push_back(verts_x[v1] * supersample_ratio_ - 0.5,
				 verts_y[v1] * supersample_ratio_ - 0.5);
  poly.push_back(verts_x[v2] * supersample_ratio_ - 0.5,
				 verts_y[v2] * supersample_ratio_ - 0.5);
  poly.push_back(verts_x[v3] * supersample_ratio_ - 0.5,
				 verts_y[v3] * supersample_ratio_ - 0.5);

  super_it_=new vgl_polygon_scan_iterator<double>(poly,false);

  poly_bb_.update(verts_x[v0], verts_y[v0]);
  poly_bb_.update(verts_x[v1], verts_y[v1]);
  poly_bb_.update(verts_x[v2], verts_y[v2]);
  poly_bb_.update(verts_x[v3], verts_y[v3]);

  int poly_xmin = (int)vcl_floor(poly_bb_.xmin());
  int poly_xmax = (int)vcl_floor(poly_bb_.xmax()) + 1; 
  poly_diameter_x_ = (poly_xmax - poly_xmin) + 1;
  aa_vals_.resize(poly_diameter_x_);
  aa_vals_offset_ = -poly_xmin;
  next_return_ = false;
}


//: Resets the scan iterator to before the first scan line
//  After calling this function, next() needs to be called before
//  startx() and endx() form a valid scan line.
void boxm_quad_scan_iterator::reset()
{
  super_it_->reset();
  next_return_ = super_it_->next();
  while(next_return_ && super_it_->scany() < 0) {
    next_return_ = super_it_->next();
  }
}

//: Tries to move to the next scan line.
//  Returns false if there are no more scan lines.
bool boxm_quad_scan_iterator::next()
{
  if (!next_return_) {
    return false;
  }
  int super_scany = super_it_->scany();
  scany_ = super_scany / supersample_ratio_; // super_scany should always be >= 0

  // initialize startx_ and endx_
  startx_ = int(poly_bb_.xmax()) + 1;
  endx_ =  int(poly_bb_.xmin()) - 1;

  // compute antialiasing values for each pixel in scanline
  vcl_fill(aa_vals_.begin(), aa_vals_.end(), 0.0f);
 
  int super_scany_end = (scany_ + 1)*supersample_ratio_;
  static const float increment = 1.0f / (supersample_ratio_*supersample_ratio_);
  static const float full_increment = 1.0f / (supersample_ratio_); 

  while ( (super_scany < super_scany_end) && next_return_ ) {
    int super_startx = super_it_->startx();
    int super_endx = super_it_->endx() + 1;

    super_startx = vcl_max(0, super_startx);
    super_endx = vcl_max(0, super_endx);
    // make sure super_startx < super_endx
    if (super_endx > super_startx) {
      int scanline_startx = super_startx / supersample_ratio_;
      int scanline_endx = ((super_endx - 1)/ supersample_ratio_) + 1;
      // update startx and endx
      if (scanline_startx < startx_) {
        startx_ = scanline_startx;
      }
      if (scanline_endx > endx_) {
        endx_ = scanline_endx;
      }
      // case 1: run covers 1 superpixel only
      if (scanline_startx + 1 == scanline_endx) {
        aa_vals_[aa_vals_offset_ + scanline_startx] += increment*(super_endx - super_startx);
      }
      // case 2: startx is less than endx by more than 1 
      else {
        // partial coverage at the beginning of scanline
        aa_vals_[aa_vals_offset_ + scanline_startx] += increment*(supersample_ratio_ - (super_startx % supersample_ratio_));

        // full coverage in middle of scanline
        for (int x = scanline_startx + 1; x < scanline_endx-1; ++x) {
          aa_vals_[aa_vals_offset_ + x] += full_increment;
        }
        // partial coverage at end of scanline
        aa_vals_[aa_vals_offset_ + scanline_endx - 1] += increment*((super_endx - 1)% supersample_ratio_ + 1);
      }
    }
    next_return_ = super_it_->next();
    super_scany = super_it_->scany();
  }
  return true;
}

//: y-coordinate of the current scan line.
int boxm_quad_scan_iterator::scany() const
{
  return scany_;
}

//: Returns starting x-value of the current scan line.
//  startx() should be smaller than endx(), unless the scan line is empty
int boxm_quad_scan_iterator::startx() const
{
  return startx_;
}

//: Returns ending x-value of the current scan line.
//  endx() should be larger than startx(), unless the scan line is empty
int  boxm_quad_scan_iterator::endx() const
{
  return endx_;
}

//: returns the amount of pixel at location x in the current scanline covered by the triangle
float boxm_quad_scan_iterator::pix_coverage(int x)
{
  return aa_vals_[aa_vals_offset_ + x];
}
boxm_quad_scan_iterator::~boxm_quad_scan_iterator()
{
delete super_it_;
}