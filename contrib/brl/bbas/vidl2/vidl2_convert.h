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
#include <vcl_cassert.h>


template <class inT, class outT>
inline void vidl2_convert_yuv2rgb(  inT  y,  inT  u,  inT  v,
                                   outT& r, outT& g, outT& b )
{

}


//: faster integer-based conversion from YUV to RGB
// based on conversion used in libdc1394
VCL_DEFINE_SPECIALIZATION 
inline void vidl2_convert_yuv2rgb( vxl_byte  y, vxl_byte  u, vxl_byte  v,
                                   vxl_byte& r, vxl_byte& g, vxl_byte& b )

{
  int iy = y, iu = u-128, iv = v-128, ir, ib, ig;
  r = ir = iy + ((iv*1436) >> 10);
  g = ig = iy - ((iu*352 + iv*731) >> 10);
  b = ib = iy + ((iu*1814) >> 10);
  r = ir < 0 ? 0 : r;
  g = ig < 0 ? 0 : g;
  b = ib < 0 ? 0 : b;
  r = ir > 255 ? 255 : r;
  g = ig > 255 ? 255 : g;
  b = ib > 255 ? 255 : b;
}


//: Wraps an existing iterator and converts YUV to RGB
template <class pixItr, class outP>
class vidl2_yuv2rgb_iterator
{
  public:
    //: Constructor
    vidl2_yuv2rgb_iterator(pixItr itr):
      itr_(itr)
    {
      vidl2_convert_yuv2rgb(itr_(0), itr_(1), itr(2), rgb_[0], rgb_[1], rgb_[2]);
    }

    //: Pre-increment
    vidl2_yuv2rgb_iterator<pixItr, outP>& operator++ ()
    {
      ++itr_;
      vidl2_convert_yuv2rgb(itr_(0), itr_(1), itr_(2), rgb_[0], rgb_[1], rgb_[2]);
      return *this;
    }

    //: Access the data
    vxl_byte operator () (unsigned int i) const
    {
      assert(i<3);
      return rgb_[i];
    }

  private:
    pixItr itr_;
    outP rgb_[3];
};



//: Use pixel iterators to unpack and copy the image pixels
// Assume that image has already been resized appropriately
template <class pixItr, class outP>
void
vidl2_convert_itr_to_view(pixItr itr, vil_image_view<outP>& image)
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
      typedef vidl2_pixel_iterator<VIDL2_PIXEL_FORMAT_YUV_422> yuvItr;
      typedef vidl2_yuv2rgb_iterator<yuvItr,outP> cvtItr;
      vidl2_convert_itr_to_view(cvtItr(yuvItr((vxl_byte*)frame->data())),image);
      return true;
    }
    default:
      break;
  }
  return false;
}





#endif // vidl2_convert_h_

