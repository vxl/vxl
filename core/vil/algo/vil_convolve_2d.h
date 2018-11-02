// This is core/vil/algo/vil_convolve_2d.h
#ifndef vil_convolve_2d_h_
#define vil_convolve_2d_h_
//:
// \file
// \brief 2D Convolution
// \author Tim Cootes

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_flip.h>
#include <vil/algo/vil_correlate_2d.h>


//: Convolve kernel with srcT
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
// On exit dest(x,y) = sum_ij src_im(x-i,y-j)*kernel(i,j)
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil_convolve_2d(const vil_image_view<srcT>& src_im,
                            vil_image_view<destT>& dest_im,
                            const vil_image_view<kernelT>& kernel,
                            accumT ac)
{
  vil_correlate_2d(src_im,dest_im,vil_flip_ud(vil_flip_lr(kernel)), ac);
}

#endif // vil_convolve_2d_h_
