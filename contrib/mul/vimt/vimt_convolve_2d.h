// This is mul/vimt/algo/vimt_convolve_2d.h
#ifndef vimt_convolve_2d_h_
#define vimt_convolve_2d_h_
//:
//  \file
//  \brief 2D Convolution
//  \author Tim Cootes

#include <vil2/algo/vil2_convolve_2d.h>
#include <vimt/vimt_image_2d_of.h>

//: Convolve kernel with src_im, keeping track of transformations
// dest is resized to (1+src_im.ni()-kernel.ni())x(1+src_im.nj()-kernel.nj())
// (a one plane image).
//
// On exit dest(x,y) = sum_ij src_im(x+i,y+j)*kernel(i,j)
//
// dest_im.world2im() is set up so that dest_im.image(x,y) is the convolution
// of the kernel with the source image when the kernel's reference point is
// placed at dest_im.world2im(x,y) in src_im
// \relates vil2_image_view
template <class srcT, class destT, class kernelT, class accumT>
inline void vil2_convolve_2d(const vimt_image_2d_of<srcT>& src_im,
                             vimt_image_2d_of<destT>& dest_im,
                             const vil2_image_view<kernelT>& kernel,
														 vgl_point_2d<double> kernel_ref_pt,
                             accumT ac)
{
  vil2_convolve_2d(src_im.image(),dest_im.image(),kernel,ac);
	vimt_transform_2d offset;
	offset.set_translation(kernel_ref_pt.x(),kernel_ref_pt.y());
  dest_im.set_world2im(offset * src_im.world2im());
	// Point (i,j) in dest corresponds to kernel placed with (0,0) at
	// (i,j) in src, so the ref point is at (i,j)+kernel_ref_pt
}

#endif // vimt_convolve_2d_h_
