// This is brl/bbas/vidl2/vidl2_convert.h
#ifndef vidl2_convert_h_
#define vidl2_convert_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Convert frames to vil_image_views
//
// \author Matt Leotta 
// \date 16 Jan 2006
//
// This file contains functions used to convert a vidl2_frame_sptr
// into a vil_image_view.


#include "vidl2_frame_sptr.h"
#include "vidl2_pixel_format.h"
#include "vidl2_pixel_iterator.h"
#include "vidl2_frame.h"
#include <vil/vil_image_view.h>


//: Use pixel iterators to unpack and copy the image pixels
// Assume that image has already been resized appropriately
template <class pixItr, class outP>
void
    vidl2_convert_to_view(pixItr itr, vil_image_view<outP>& image)
{
  for (unsigned j = 0; j < image.nj(); ++j)
    for (unsigned i = 0; i < image.ni(); ++i, ++itr)
      for (unsigned p = 0; p < image.nplanes(); ++p)
        image(i,j,p) = static_cast<outP>( itr(p) );
}


//: Unpack the pixels in the \p frame into the memory of the \p image
// The image is resized if needed.
template <class outP>
bool vidl2_convert_to_view(const vidl2_frame_sptr& frame,
                           vil_image_view<outP>& image)
{
  vidl2_pixel_format fmt = frame->pixel_format();
  if(fmt == VIDL2_PIXEL_FORMAT_UNKNOWN)
    return false;

  vidl2_pixel_traits traits = vidl2_pixel_format_traits(fmt);

  unsigned ni=frame->ni();
  unsigned nj=frame->nj();
  unsigned np=traits.num_channels;

  image.set_size(ni,nj,np);

  switch(fmt){
    case VIDL2_PIXEL_FORMAT_YUV_422:
    {
      vidl2_convert_to_view(vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422>((vxl_byte*)frame->data()),image);
      return true;
    }
    default:
      break;
  }
  return false;
}





#endif // vidl2_convert_h_

