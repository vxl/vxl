// This is mul/vimt/algo/vimt_correlate_2d.h
#ifndef vimt_correlate_2d_h_
#define vimt_correlate_2d_h_
//:
//  \file
//  \brief 2D Convolution
//  \author Tim Cootes

#include <vil/algo/vil_correlate_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_fwd.h>

//: Correlate kernel with src_im, keeping track of transformations.
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
//
// On exit dest(x,y) = sum_ij src_im(x+i,y+j)*kernel(i,j)
//
// dest_im.world2im() is set up so that vimt_bilin_interp(dest_im,p) is the convolution
// of the kernel with the source image when the kernel's reference point is
// placed at p in src_im
// \relatesalso vil_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vimt_correlate_2d(const vimt_image_2d_of<srcT>& src_im,
                              vimt_image_2d_of<destT>& dest_im,
                              const vil_image_view<kernelT>& kernel,
                              vgl_point_2d<double> kernel_ref_pt,
                              accumT ac)
{
  vil_correlate_2d(src_im.image(),dest_im.image(),kernel,ac);
  vimt_transform_2d offset;
  offset.set_translation(-kernel_ref_pt.x(),-kernel_ref_pt.y());
  dest_im.set_world2im(offset * src_im.world2im());
}

#endif // vimt_correlate_2d_h_
