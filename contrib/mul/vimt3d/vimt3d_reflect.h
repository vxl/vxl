// This is mul/vimt3d/vimt3d_reflect.h
#ifndef vimt3d_reflect_h_
#define vimt3d_reflect_h_
//:
// \file
// \author Kevin de Souza
// \brief Functions to reflect a vimt3d_image_3d_of<T>


#include <vil3d/vil3d_reflect.h>
#include <vimt3d/vimt3d_image_3d_of.h>


//: Reflect an image about the x=0 plane (world coords)
template<class T>
inline void vimt3d_reflect_x(vimt3d_image_3d_of<T>& img)
{
  // Transpose the voxel array. This is equivalent to reflecting about the image centre.
  img.image() = vil3d_reflect_i(img.image());

  // Compose the transform with a translation about the world origin.
  std::vector<double> bboxmin, bboxmax;
  img.world_bounds(bboxmin, bboxmax);
  vimt3d_transform_3d transl;
  transl.set_translation(bboxmin[0]+bboxmax[0], 0, 0);
  img.world2im() = img.world2im() * transl;
}


//: Reflect an image about the y=0 plane (world coords)
template<class T>
inline void vimt3d_reflect_y(vimt3d_image_3d_of<T>& img)
{
  // Transpose the voxel array. This is equivalent to reflecting about the image centre.
  img.image() = vil3d_reflect_j(img.image());

  // Compose the transform with a translation about the world origin.
  std::vector<double> bboxmin, bboxmax;
  img.world_bounds(bboxmin, bboxmax);
  vimt3d_transform_3d transl;
  transl.set_translation(0, bboxmin[1]+bboxmax[1], 0);
  img.world2im() = img.world2im() * transl;
}


//: Reflect an image about the z=0 plane (world coords)
template<class T>
inline void vimt3d_reflect_z(vimt3d_image_3d_of<T>& img)
{
  // Transpose the voxel array. This is equivalent to reflecting about the image centre.
  img.image() = vil3d_reflect_k(img.image());

  // Compose the transform with a translation about the world origin.
  std::vector<double> bboxmin, bboxmax;
  img.world_bounds(bboxmin, bboxmax);
  vimt3d_transform_3d transl;
  transl.set_translation(0, 0, bboxmin[2]+bboxmax[2]);
  img.world2im() = img.world2im() * transl;
}


#endif // vimt3d_reflect_h_
