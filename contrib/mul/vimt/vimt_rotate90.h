// This is mul/vimt/vimt_rotate90.h
#ifndef vimt_rotate90_h_
#define vimt_rotate90_h_
//:
// \file
// \brief Functions to rotate an image by 90,180,270 degrees by manipulating pointers
// \author Tim Cootes
// No data is moved, so very fast.

#include <vimt/vimt_image_2d_of.h>

//: Create view which rotates image by 90 degrees
template<class T>
vimt_image_2d_of<T> vimt_rotate90(const vimt_image_2d_of<T>& image)
{
  const vil_image_view<T>& im=image.image();
  unsigned ni=im.ni(),nj=im.nj();
  vil_image_view<T> new_im(&im(ni-1,0), nj,ni,im.nplanes(), im.jstep(),-im.istep(),im.planestep());

  vimt_transform_2d i2n;
  i2n.set_similarity(vgl_vector_2d<double>(0,-1),vgl_point_2d<double>(0,ni-1));

  return vimt_image_2d_of<T>(new_im,i2n*image.world2im());
}

//: Create view which rotates image by 180 degrees
template<class T>
vimt_image_2d_of<T> vimt_rotate180(const vimt_image_2d_of<T>& image)
{
  const vil_image_view<T>& im=image.image();
  unsigned ni=im.ni(),nj=im.nj();
  vil_image_view<T> new_im(&im(ni-1,nj-1), ni,nj,im.nplanes(), -im.istep(),-im.jstep(),im.planestep());

  vimt_transform_2d i2n;
  i2n.set_similarity(vgl_vector_2d<double>(-1,0),vgl_point_2d<double>(ni-1,nj-1));

  return vimt_image_2d_of<T>(new_im,i2n*image.world2im());
}


//: Create view which rotates image by 270 degrees
template<class T>
vimt_image_2d_of<T> vimt_rotate270(const vimt_image_2d_of<T>& image)
{
  const vil_image_view<T>& im=image.image();
  unsigned ni=im.ni(),nj=im.nj();
  vil_image_view<T> new_im(&im(0,nj-1), nj,ni,im.nplanes(), -im.jstep(),im.istep(),im.planestep());

  vimt_transform_2d i2n;
  i2n.set_similarity(vgl_vector_2d<double>(0,1),vgl_point_2d<double>(nj-1,0));

  return vimt_image_2d_of<T>(new_im,i2n*image.world2im());
}

//: Create view which rotates image by 90*i degrees
template<class T>
vimt_image_2d_of<T> vimt_rotate_n90(const vimt_image_2d_of<T>& image, unsigned i)
{
  switch (i%4)
  {
    case 0: return image;
    case 1: return vimt_rotate90(image);
    case 2: return vimt_rotate180(image);
    case 3: return vimt_rotate270(image);
  }
  return image;
}
#endif // vimt_rotate90_h_
