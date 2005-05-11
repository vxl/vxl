//:
//  \file
//  \brief A base class for arbitrary 2D images+transform
//  \author Tim Cootes

#include "vimt_image_2d.h"

//: Return vector indicating size of image in pixels
//  2D image is v[0] x v[1],  3D image is v[0] x v[1] x v[2]
//  Somewhat inefficient: Only use when you have to
vcl_vector<unsigned> vimt_image_2d::image_size() const
{
  vcl_vector<unsigned> d(2);
  d[0]=image_base().ni();
  d[1]=image_base().nj();
  return d;
}

//: Return vectors defining bounding box containing image in world co-ords
void vimt_image_2d::world_bounds(vcl_vector<double>& b_lo,
                                 vcl_vector<double>& b_hi) const
{
  b_lo.resize(2); b_hi.resize(2);
  vgl_point_2d<double> p = world2im_.inverse()(0,0);
  b_lo[0]=p.x(); b_hi[0]=p.x();
  b_lo[1]=p.y(); b_hi[1]=p.y();

  // Compute each corner
  for (int i=0;i<2;++i)
    for (int j=0;j<2;++j)
    {
      p = world2im_.inverse()(i*(image_base().ni()-1),j*(image_base().nj()-1));
      if (p.x()<b_lo[0]) b_lo[0]=p.x();
      else if (p.x()>b_hi[0]) b_hi[0]=p.x();
      if (p.y()<b_lo[1]) b_lo[1]=p.y();
      else if (p.y()>b_hi[1]) b_hi[1]=p.y();
    }
}


//Related Functions
//: Return bounding box containing image in world co-ords as a box
vgl_box_2d<double> world_bounding_box(const vimt_image_2d& img) 
{
          
    vcl_vector<double> b_lo(2,0.0);
    vcl_vector<double> b_hi(2,0.0);
    img.world_bounds(b_lo,b_hi);      
    return vgl_box_2d<double>(b_lo[0],b_hi[0],b_lo[1],b_hi[1]);
}
