// This is core/vidl/vidl_frame.cxx
//:
// \file
// \author Matt Leotta
// \date   13 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl_frame.h"
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vil/vil_image_view.h>

//-----------------------------------------------------------------------------

//: Decrement reference count
void
vidl_frame::unref()
{
  assert (ref_count_ >0);
  ref_count_--;
  if (ref_count_==0)
  {
    delete this;
  }
}

//-----------------------------------------------------------------------------

//: Constructor - from a vil_image_view
// return an invalid frame if the image format can not be wrapped
vidl_memory_chunk_frame::
vidl_memory_chunk_frame(const vil_image_view_base& image,
                        vidl_pixel_format fmt)
  : vidl_frame(), memory_(nullptr)
{
  ni_ = image.ni();
  nj_ = image.nj();
  // use the pixel component format to account for
  // images of type vil_rgb<T>, vil_rgba<T>, etc.
  vil_pixel_format cmp_format =
      vil_pixel_format_component_format(image.pixel_format());
  unsigned int num_cmp = vil_pixel_format_num_components(image.pixel_format());
  unsigned int num_channels = image.nplanes() * num_cmp;

  if (cmp_format == VIL_PIXEL_FORMAT_UINT_16 &&
      num_channels == 1)
  {
    vil_image_view<vxl_uint_16> img = image;
    if (!img.is_contiguous())
      return;
    memory_ = img.memory_chunk();
    format_ = VIDL_PIXEL_FORMAT_MONO_16;
  }
  else if (cmp_format == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte> img = image;
    if (!img.is_contiguous())
      return;
    memory_ = img.memory_chunk();
    if (img.nplanes() == 1)
    {
      format_ = VIDL_PIXEL_FORMAT_MONO_8;
    }
    else if (img.nplanes() == 3)
    {
      if (img.planestep() == 1) {
        if (fmt == VIDL_PIXEL_FORMAT_UYV_444)
          format_ = VIDL_PIXEL_FORMAT_UYV_444;
        else
          format_ = VIDL_PIXEL_FORMAT_RGB_24;
      }
      else
      {
        if (fmt == VIDL_PIXEL_FORMAT_YUV_444P)
          format_ = VIDL_PIXEL_FORMAT_YUV_444P;
        else
          format_ = VIDL_PIXEL_FORMAT_RGB_24P;
      }
    }
    else if (img.nplanes() == 4)
    {
      if (img.planestep() == 1)
        format_ = VIDL_PIXEL_FORMAT_RGBA_32;
      else
        format_ = VIDL_PIXEL_FORMAT_RGBA_32P;
    }
  }
  else if ( cmp_format == VIL_PIXEL_FORMAT_FLOAT )
  {
    vil_image_view<vxl_ieee_32> img = image;
    if (!img.is_contiguous())
      return;
    memory_ = img.memory_chunk();
    if ( img.nplanes()==1 )
      format_ = VIDL_PIXEL_FORMAT_MONO_F32;
    else if ( img.nplanes()==3 ) {
      if (img.planestep() == 1)
        format_ = VIDL_PIXEL_FORMAT_RGB_F32;
      else
        format_ = VIDL_PIXEL_FORMAT_RGB_F32P;
    }
  }

  if (fmt != VIDL_PIXEL_FORMAT_UNKNOWN &&
      fmt != format_)
    format_ = VIDL_PIXEL_FORMAT_UNKNOWN;

  if (format_ == VIDL_PIXEL_FORMAT_UNKNOWN)
    memory_ = nullptr;
}
