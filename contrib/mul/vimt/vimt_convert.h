// This is mul/vimt/vimt_convert.h
#ifndef vimt_convert_h_
#define vimt_convert_h_

//:
//  \file
//  \brief Various type conversion functions for vimt_image objects
//  \author Tim Cootes
// Note: There is a general rule that we should not duplicate 
// vil classes or functions with trivial wrappers in vimt.
// These conversion functions break that rule because they are
// used so frequently.

#include <vimt/vimt_image_2d_of.h>
#include <vil/vil_convert.h>

//: Converts by applying a cast
// If the two pixel types are the same, the destination may only be a
// shallow copy of the source.
template<class srcT, class destT>
void vimt_convert_cast(const vimt_image_2d_of<srcT>& src_im,
                       vimt_image_2d_of<destT>& dest_im)
{
  vil_convert_cast(src_im.image(),dest_im.image());
  dest_im.set_world2im(src_im.world2im());
}

//: Converts by rounding
// If the two pixel types are the same, the destination may only be a
// shallow copy of the source.
template<class srcT, class destT>
void vimt_convert_round(const vimt_image_2d_of<srcT>& src_im,
                       vimt_image_2d_of<destT>& dest_im)
{
  vil_convert_round(src_im.image(),dest_im.image());
  dest_im.set_world2im(src_im.world2im());
}

//: Convert src to byte image dest by stretching to range [0,255]
template<class srcT>
void vimt_convert_stretch_range(const vimt_image_2d_of<srcT>& src_im,
                                vimt_image_2d_of<vxl_byte>& dest_im)
{
  vil_convert_stretch_range(src_im.image(),dest_im.image());
  dest_im.set_world2im(src_im.world2im());
}



#endif // vimt_convert_h_
