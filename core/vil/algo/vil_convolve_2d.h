// This is mul/vil2/algo/vil2_convolve_2d.h
#ifndef vil2_convolve_2d_h_
#define vil2_convolve_2d_h_
//:
//  \file
//  \brief 2D Convolution
//  \author Tim Cootes

#include <vcl_compiler.h>
#include <vcl_cassert.h>
#include <vil2/vil2_flip.h>
#include <vil2/algo/vil2_correlate_2d.h>


//: Convolve kernel with srcT
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
// On exit dest(x,y) = sum_ij src_im(x-i,y-j)*kernel(i,j)
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_convolve_2d(const vil2_image_view<srcT>& src_im,
                                  vil2_image_view<destT>& dest_im,
                                  const vil2_image_view<kernelT>& kernel,
                                  accumT ac)
{
  vil2_correlate_2d(src_im,dest_im,vil2_flip_ud(vil2_flip_lr(kernel)), ac);
}
#endif // vil2_convolve_2d_h_
