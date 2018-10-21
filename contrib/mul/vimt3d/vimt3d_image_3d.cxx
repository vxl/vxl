//:
//  \file
//  \brief A base class for arbitrary 3d images+transform
//  \author Tim Cootes

#include "vimt3d_image_3d.h"
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

//: Return vector indicating size of image in pixels
//  3D image is v[0] x v[1] x v[2]
std::vector<unsigned> vimt3d_image_3d::image_size() const
{
  std::vector<unsigned> d(3);
  d[0]=image_base().ni();
  d[1]=image_base().nj();
  d[2]=image_base().nk();
  return d;
}


//: Return 3 element vector indicating the size of a pixel
std::vector<double> vimt3d_image_3d::pixel_size() const
{
  std::vector<double> d(3);
  vgl_vector_3d<double> v =world2im_.inverse().
    delta(vgl_point_3d<double>(0,0,0), vgl_vector_3d<double>(1.0,1.0,1.0));

  d[0] = v.x();
  d[1] = v.y();
  d[2] = v.z();
  return d;
}


//: Return vectors defining bounding box containing image in world co-ords
void vimt3d_image_3d::world_bounds(std::vector<double>& b_lo,
                                   std::vector<double>& b_hi) const
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

//Related Functions
//: Return bounding box containing image in world co-ords as a box
vgl_box_3d<double> world_bounding_box(const vimt3d_image_3d& img)
{
  std::vector<double> b_lo(3,0.0);
  std::vector<double> b_hi(3,0.0);
  img.world_bounds(b_lo,b_hi);
  //Use C-style vector interface for corner points, passing address of data as the C-style vector
  return {&(b_lo[0]),&(b_hi[0])};
}

// Translate the image so that its centre is at the origin of the world coordinate system.
void vimt3d_centre_image_at_origin(vimt3d_image_3d& image)
{
  vgl_box_3d<double> bbox = world_bounding_box(image);
  vgl_point_3d<double> c = bbox.centroid();
  vimt3d_transform_3d& w2i = image.world2im();
  w2i.set_origin(w2i(c));
}

// Calculate the voxel dimensions from the image transform
// NEEDS A TEST PROGRAM
vgl_vector_3d<double> vimt3d_voxel_size_from_transform(const vimt3d_image_3d& image)
{
  const vimt3d_transform_3d& i2w = image.world2im().inverse();
  vgl_point_3d<double> p(0,0,0);
  vgl_vector_3d<double> i(1,0,0);
  vgl_vector_3d<double> j(0,1,0);
  vgl_vector_3d<double> k(0,0,1);
  double dx = i2w.delta(p, i).length();
  double dy = i2w.delta(p, j).length();
  double dz = i2w.delta(p, k).length();
  return {dx, dy, dz};
}
