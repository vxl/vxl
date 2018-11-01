// This is mul/vimt/vimt_reflect.h
#ifndef vimt_reflect_h_
#define vimt_reflect_h_
//:
// \file
// \author Kevin de Souza
// \brief Functions to reflect a vimt_image_2d_of<T>


#include <vil/vil_flip.h>
#include <vimt/vimt_image_2d_of.h>


//: Reflect an image about the x=0 line (world coords)
template<class T>
inline void vimt_reflect_x(vimt_image_2d_of<T>& img)
{
  // Transpose the pixel array. This is equivalent to reflecting about the image centre.
  img.image() = vil_flip_lr(img.image());

  // Compose the transform with a translation about the world origin.
  std::vector<double> bboxmin, bboxmax;
  img.world_bounds(bboxmin, bboxmax);
  vimt_transform_2d transl;
  transl.set_translation(bboxmin[0]+bboxmax[0], 0);
  img.world2im() = img.world2im() * transl;
}


//: Reflect an image about the y=0 line (world coords)
template<class T>
inline void vimt_reflect_y(vimt_image_2d_of<T>& img)
{
  // Transpose the pixel array. This is equivalent to reflecting about the image centre.
  img.image() = vil_flip_ud(img.image());

  // Compose the transform with a translation about the world origin.
  std::vector<double> bboxmin, bboxmax;
  img.world_bounds(bboxmin, bboxmax);
  vimt_transform_2d transl;
  transl.set_translation(0, bboxmin[1]+bboxmax[1]);
  img.world2im() = img.world2im() * transl;
}


#endif // vimt_reflect_h_
