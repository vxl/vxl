// This is algo/bapl/bapl_affine_roi.cxx
//:
// \file

#include "bapl_affine_roi.h"


//: Constructor
bapl_affine_roi::bapl_affine_roi(vil_image_view<vxl_byte> src_image,
                                 bapl_affine_transform xform, 
                                 unsigned ni, unsigned nj )
  :  rectified_valid_(false), src_img_(src_image), rectified_img_(ni,nj),
     xform_(xform)
{}


//: Return the rectified image (calculate as needed)
const vil_image_view<vxl_byte>& 
bapl_affine_roi::rectified_image()
{
  if(!rectified_valid_)
    this->rectify();
  return rectified_img_;
}


//: Map the affine ROI into a rectangular base image
void
bapl_affine_roi::rectify()
{

  unsigned ni=rectified_img_.ni(), nj=rectified_img_.nj();
  vcl_ptrdiff_t istep=rectified_img_.istep(), jstep=rectified_img_.jstep();

  vxl_byte* row = rectified_img_.top_left_ptr();
  for (unsigned j=0; j<nj; ++j,row+=jstep){
    vxl_byte* pixel = row;
    for (unsigned i=0; i<ni; ++i,pixel+=istep){
      double x,y;
      xform_.map(i,j, x,y);
      if( x>=0 && x<src_img_.ni() && y>=0 && y<src_img_.nj() )
        *pixel = src_img_(int(x), int(y));
      else
        *pixel = 0;
    }
  }

  rectified_valid_ = true;
}
