// This is mul/vimt3d/vimt3d_gauss_reduce.h
#ifndef vimt3d_gauss_reduce_h_
#define vimt3d_gauss_reduce_h_
//:
// \file
// \brief Function to smooth and sub-sample 3D images
// \author Kevin de Souza


#include <vil3d/algo/vil3d_gauss_reduce.h>


//: Smooth and subsample src_im to produce dest_im
//  Applies filter in i,j and k directions, then samples every other pixel.
//  Resulting image is (ni+1)/2 x (nj+1)/2 x (nk+1)/2. 
//  Transform is modified by a scaling factor of 0.5.
//  An image can be reduced in-place, by having src_im and dest_im pointing to the same image.
//  Requires client to provide 2 workspace images.
// \sa vimt3d_gauss_reduce()
template<class T>
void vimt3d_gauss_reduce(const vimt3d_image_3d_of<T>& src,
                         vimt3d_image_3d_of<T>&       dst,
                         vimt3d_image_3d_of<T>&       work1,
                         vimt3d_image_3d_of<T>&       work2)
{
  vil3d_gauss_reduce(src.image(), dst.image(), work1.image(), work2.image());

  vimt3d_transform_3d scaling;
  scaling.set_zoom_only(0.5, 0.5, 0.5, 0.0, 0.0, 0.0);
  dst.set_world2im(scaling * src.world2im());
}


//: Smooth and subsample src_im to produce dest_im
//  This simple overload does not require client to provide 2 workspace images.
// \sa vimt3d_gauss_reduce()
template<class T>
void vimt3d_gauss_reduce(const vimt3d_image_3d_of<T>& src,
                         vimt3d_image_3d_of<T>&       dst)
{
  // Workspace images are resized as required by vil3d_gauss_reduce()
  vimt3d_image_3d_of<T> work1;  
  vimt3d_image_3d_of<T> work2;
  vimt3d_gauss_reduce(src, dst, work1, work2);
}


#endif // vimt3d_gauss_reduce_h_
