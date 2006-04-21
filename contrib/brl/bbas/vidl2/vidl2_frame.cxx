// This is brl/bbas/vidl2/vidl2_frame.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Matt Leotta
// \date   13 Jan 2006
//
//-----------------------------------------------------------------------------

#include "vidl2_frame.h"
#include <vcl_cassert.h>
#include <vil/vil_image_view.h>

//-----------------------------------------------------------------------------

//: Decrement reference count
void
vidl2_frame::unref()
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
vidl2_memory_chunk_frame::
vidl2_memory_chunk_frame(const vil_image_view_base& image,
                         vidl2_pixel_format fmt)
  : vidl2_frame(), memory_(NULL)
{
  ni_ = image.ni();
  nj_ = image.nj();
  if (image.pixel_format() == VIL_PIXEL_FORMAT_UINT_16 &&
     image.nplanes() == 1)
  {
    vil_image_view<vxl_uint_16> img = image;
    if(!img.is_contiguous())
      return;
    memory_ = img.memory_chunk();
    format_ = VIDL2_PIXEL_FORMAT_MONO_16;
  }
  else if (image.pixel_format() == VIL_PIXEL_FORMAT_BYTE)
  {
    vil_image_view<vxl_byte> img = image;
    if(!img.is_contiguous())
      return;
    memory_ = img.memory_chunk();
    if (img.nplanes() == 1)
    {
      format_ = VIDL2_PIXEL_FORMAT_MONO_8;
    }
    if (img.nplanes() == 3)
    {
      if (img.planestep() == 1){
        if(fmt == VIDL2_PIXEL_FORMAT_UYV_444)
          format_ = VIDL2_PIXEL_FORMAT_UYV_444;
        else
          format_ = VIDL2_PIXEL_FORMAT_RGB_24;
      }
      else
      {
        if(fmt == VIDL2_PIXEL_FORMAT_YUV_444P)
          format_ = VIDL2_PIXEL_FORMAT_YUV_444P;
        else
          format_ = VIDL2_PIXEL_FORMAT_RGB_24P;
      }
    }
    if (img.nplanes() == 4)
    {
      if (img.planestep() == 1)
        format_ = VIDL2_PIXEL_FORMAT_RGBA_32;
      else
        format_ = VIDL2_PIXEL_FORMAT_RGBA_32P;
    }
  }

  if(fmt != VIDL2_PIXEL_FORMAT_UNKNOWN &&
     fmt != format_)
    format_ = VIDL2_PIXEL_FORMAT_UNKNOWN;

  if(format_ == VIDL2_PIXEL_FORMAT_UNKNOWN)
    memory_ = NULL;
}


