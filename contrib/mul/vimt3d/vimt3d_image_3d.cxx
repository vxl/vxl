//:
//  \file
//  \brief A base class for arbitrary 3d images+transform
//  \author Tim Cootes

#include "vimt3d_image_3d.h"

//: Return vector indicating size of image in pixels
//  3D image is v[0] x v[1] x v[2]
vcl_vector<unsigned> vimt3d_image_3d::image_size() const
{
  vcl_vector<unsigned> d(3);
  d[0]=image_base().ni();
  d[1]=image_base().nj();
  d[2]=image_base().nk();
  return d;
}

//: Return vectors defining bounding box containing image in world co-ords
void vimt3d_image_3d::world_bounds(vcl_vector<double>& b_lo,
                                   vcl_vector<double>& b_hi) const
{
  b_lo.resize(3); b_hi.resize(3);
  vgl_point_3d<double> p = world2im_.inverse()(0,0,0);
  b_lo[0]=p.x(); b_hi[0]=p.x();
  b_lo[1]=p.y(); b_hi[1]=p.y();
  b_lo[2]=p.z(); b_hi[2]=p.z();

  // Compute each corner
  for (int i=0;i<2;++i)
    for (int j=0;j<2;++j)
      for (int k=0;k<2;++k)
      {
        p = world2im_.inverse()(i*(image_base().ni()-1),
                                j*(image_base().nj()-1),
                                k*(image_base().nk()-1));
        if (p.x()<b_lo[0]) b_lo[0]=p.x();
        else if (p.x()>b_hi[0]) b_hi[0]=p.x();
        if (p.y()<b_lo[1]) b_lo[1]=p.y();
        else if (p.y()>b_hi[1]) b_hi[1]=p.y();
        if (p.z()<b_lo[2]) b_lo[2]=p.z();
        else if (p.z()>b_hi[2]) b_hi[2]=p.z();
      }
}
