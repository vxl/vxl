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


// ITU-R BT.601 (formerly CCIR 601) standard conversion
template <class outT>
inline void vidl2_convert_yuv2rgb( vxl_byte y, vxl_byte u, vxl_byte v,
                                      outT& r,    outT& g,    outT& b )
{
  double dy = y/255.0;       // 0.0 to 1.0
  double du = (u-128)/255.0; //-0.5 to 0.5
  double dv = (v-128)/255.0; //-0.5 to 0.5
  r = dy + 1.1402 * dv;
  g = dy - 0.34413628620102 * du - 0.71413628620102 * dv;
  b = dy + 1.772 * du;
}


//: faster integer-based conversion from YUV to RGB
// based on conversion used in libdc1394
VCL_DEFINE_SPECIALIZATION 
inline void vidl2_convert_yuv2rgb( vxl_byte  y, vxl_byte  u, vxl_byte  v,
                                   vxl_byte& r, vxl_byte& g, vxl_byte& b )

{
  register int iy = y, iu = u-128, iv = v-128, ir, ib, ig;
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
    outP operator () (unsigned int i) const
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
  unsigned ni = image.ni();
  unsigned nj = image.nj();
  unsigned np = image.nplanes();
  vcl_ptrdiff_t istep = image.istep(); 
  vcl_ptrdiff_t jstep = image.jstep();
  vcl_ptrdiff_t pstep = image.planestep();

  outP* row = image.top_left_ptr();
  for (unsigned int j=0; j<nj; ++j, row+=jstep)
  {
    outP* col = row;
    for (unsigned int i=0; i<ni; ++i, col+=istep, ++itr)
    {
      outP* pixel = col;
      for (unsigned int p=0; p<np; ++p, pixel+=pstep)
      {
        *pixel = static_cast<outP>( itr(p) );
      }
    }
  }

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

#define do_case(FMT) case FMT:\
    {\
      typedef vidl2_pixel_iterator<FMT> Itr;\
      vidl2_convert_itr_to_view(Itr(frame),image);\
      return true;\
    }
  switch(fmt){
    do_case(VIDL2_PIXEL_FORMAT_RGB_24);
    do_case(VIDL2_PIXEL_FORMAT_BGR_24);
    do_case(VIDL2_PIXEL_FORMAT_RGBA_32);
    do_case(VIDL2_PIXEL_FORMAT_RGB_565);
    do_case(VIDL2_PIXEL_FORMAT_RGB_555);

    do_case(VIDL2_PIXEL_FORMAT_YUV_444);
    do_case(VIDL2_PIXEL_FORMAT_YUV_444P);
    do_case(VIDL2_PIXEL_FORMAT_YUV_422);
    do_case(VIDL2_PIXEL_FORMAT_YUV_422P);
    do_case(VIDL2_PIXEL_FORMAT_YUV_420P);
    do_case(VIDL2_PIXEL_FORMAT_YUV_411);
    do_case(VIDL2_PIXEL_FORMAT_YUV_411P);
    do_case(VIDL2_PIXEL_FORMAT_YUV_410P);
    do_case(VIDL2_PIXEL_FORMAT_UYVY_422);
    do_case(VIDL2_PIXEL_FORMAT_UYVY_411);

    do_case(VIDL2_PIXEL_FORMAT_MONO_1);
    do_case(VIDL2_PIXEL_FORMAT_MONO_8);
    do_case(VIDL2_PIXEL_FORMAT_MONO_16);

    default:
      break;
  }
#undef do_case
  return false;
}


//: Unpack the pixels in the \p frame convert to RGB and store in \p image
// Converts non-RGB color modes into RGB, monochrome is unchanged
// The image is resized if needed.
template <class outP>
bool vidl2_convert_to_view_rgb(const vidl2_frame_sptr& frame,
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

#define do_case(FMT) case FMT:\
    {\
      typedef vidl2_pixel_iterator<FMT> Itr;\
      vidl2_convert_itr_to_view(Itr(frame),image);\
      return true;\
    }

#define do_case_yuv2rgb(FMT) case FMT:\
    {\
      typedef vidl2_pixel_iterator<FMT> yuvItr;\
      typedef vidl2_yuv2rgb_iterator<yuvItr,outP> cvtItr;\
      vidl2_convert_itr_to_view(cvtItr(yuvItr(frame)),image);\
      return true;\
    }
  switch(fmt){
    do_case(VIDL2_PIXEL_FORMAT_RGB_24);
    do_case(VIDL2_PIXEL_FORMAT_BGR_24);
    do_case(VIDL2_PIXEL_FORMAT_RGBA_32);
    do_case(VIDL2_PIXEL_FORMAT_RGB_565);
    do_case(VIDL2_PIXEL_FORMAT_RGB_555);

    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_444);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_444P);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_422);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_422P);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_420P);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_411);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_411P);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_YUV_410P);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_UYVY_422);
    do_case_yuv2rgb(VIDL2_PIXEL_FORMAT_UYVY_411);

    do_case(VIDL2_PIXEL_FORMAT_MONO_1);
    do_case(VIDL2_PIXEL_FORMAT_MONO_8);
    do_case(VIDL2_PIXEL_FORMAT_MONO_16);

    default:
      break;
  }
#undef do_case
#undef do_case_yuv2rgb

  return false;
}




#endif // vidl2_convert_h_

