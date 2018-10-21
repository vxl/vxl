//:
//  \file
//  \brief A base class for arbitrary 2D images+transform
//  \author Tim Cootes

#include "vimt_image_2d.h"
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>

//: Return vector indicating size of image in pixels
//  2D image is v[0] x v[1],  3D image is v[0] x v[1] x v[2]
//  Somewhat inefficient: Only use when you have to
std::vector<unsigned> vimt_image_2d::image_size() const
{
  std::vector<unsigned> d(2);
  d[0]=image_base().ni();
  d[1]=image_base().nj();
  return d;
}


//: Return 2 element vector indicating the size of a pixel
std::vector<double> vimt_image_2d::pixel_size() const
{
  std::vector<double> d(2);
  vgl_vector_2d<double> v =world2im_.inverse().
    delta(vgl_point_2d<double>(0,0), vgl_vector_2d<double>(1.0,1.0));

  d[0] = v.x();
  d[1] = v.y();
  return d;
}


//: Return vectors defining bounding box containing image in world co-ords
void vimt_image_2d::world_bounds(std::vector<double>& b_lo,
                                 std::vector<double>& b_hi) const
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


// Related Functions

// Return bounding box containing image in world co-ords as a box
vgl_box_2d<double> world_bounding_box(const vimt_image_2d& img)
{
  std::vector<double> b_lo(2,0.0);
  std::vector<double> b_hi(2,0.0);
  img.world_bounds(b_lo,b_hi);
  return {b_lo[0],b_hi[0],b_lo[1],b_hi[1]};
}

// Translate the image so that its centre is at the origin of the world coordinate system.
void vimt_centre_image_at_origin(vimt_image_2d& image)
{
  vgl_box_2d<double> bbox = world_bounding_box(image);
  vgl_point_2d<double> c = bbox.centroid();
  vimt_transform_2d& w2i = image.world2im();
  w2i.set_origin(w2i(c));
}

// Calculate the pixel dimensions from the image transform
// NEEDS A TEST PROGRAM
vgl_vector_2d<double> vimt_pixel_size_from_transform(const vimt_image_2d& image)
{
  const vimt_transform_2d& i2w = image.world2im().inverse();
  vgl_point_2d<double> p(0,0);
  vgl_vector_2d<double> i(1,0);
  vgl_vector_2d<double> j(0,1);
  double dx = i2w.delta(p, i).length();
  double dy = i2w.delta(p, j).length();
  return {dx, dy};
}
