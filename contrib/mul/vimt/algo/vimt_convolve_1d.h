// This is mul/vimt/algo/vimt_convolve_1d.h
#ifndef vimt_convolve_1d_h_
#define vimt_convolve_1d_h_
//:
//  \file
//  \brief Convolution with 1D kernel
//  \author Tim Cootes

#include <vil2/algo/vil2_convolve_1d.h>
#include <vimt/vimt_image_2d_of.h>

//: Convolve kernel with src_im, keeping track of transformations
// Convolve kernel[i] (i in [k_lo,k_hi]) with srcT in i-direction
// On exit dest_im(i,j) = sum src(i+x,j)*kernel(x)  (x=k_lo..k_hi)
// \param kernel should point to tap 0.
// dest_im resized to size of src_im.
//
// dest_im.world2im() is set up so that vimt_bilin_interp(dest_im,p) is the convolution
// of the kernel with the source image when the kernel's reference point is
// placed at p in src_im
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vimt_convolve_1d(const vimt_image_2d_of<srcT>& src_im,
                             vimt_image_2d_of<destT>& dest_im,
                             const kernelT* kernel, int k_lo, int k_hi,
                             accumT ac,
                             vil2_convolve_boundary_option start_option,
                             vil2_convolve_boundary_option end_option)
{
  vil2_convolve_1d(src_im.image(),dest_im.image(),kernel,k_lo,k_hi,ac,
                   start_option,end_option);
  dest_im.set_world2im(src_im.world2im());
}

#endif // vimt_convolve_1d_h_
