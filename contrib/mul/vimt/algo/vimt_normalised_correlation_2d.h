// This is mul/vimt/algo/vimt_normalised_correlation_2d.h
#ifndef vimt_normalised_correlation_2d_h_
#define vimt_normalised_correlation_2d_h_
//:
//  \file
//  \brief 2D Normalised correlation
//  \author Tim Cootes

#include <vil/algo/vil_normalised_correlation_2d.h>
#include <vimt/vimt_image_2d_of.h>
#include <vgl/vgl_fwd.h>

//: Evaluate dot product between kernel and (normalised) src_im
// Assumes that the kernel has been normalised to have zero mean
// and unit variance.
// \relatesalso vimt_image_2d_of
template <class srcT, class destT, class kernelT, class accumT>
inline void vimt_normalised_correlation_2d(const vimt_image_2d_of<srcT>& src_im,
                                           vimt_image_2d_of<destT>& dest_im,
                                           const vil_image_view<kernelT>& kernel,
                                           vgl_point_2d<double> kernel_ref_pt,
                                           accumT ac)
{
  vil_normalised_correlation_2d(src_im.image(),dest_im.image(),kernel,ac);
  vimt_transform_2d offset;
  offset.set_translation(-kernel_ref_pt.x(),-kernel_ref_pt.y());
  dest_im.set_world2im(offset * src_im.world2im());
}

#endif // vimt_normalised_correlation_2d_h_
