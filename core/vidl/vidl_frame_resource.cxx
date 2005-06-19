// This is core/vidl/vidl_frame_resource.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vidl_frame_resource.h"

#include <vcl_cassert.h>
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_stream.h>
#include <vil/vil_image_view.h>
#include <vil/vil_property.h>

#include <vcl_cstdlib.h> // for vcl_exit()
#include <vxl_config.h>


vidl_frame_resource::vidl_frame_resource(const vidl_codec_sptr& codec, int frame)
 : codec_(codec),
   frame_number_(frame)
{
}


vidl_frame_resource::vidl_frame_resource(const vidl_frame& frame)
 : codec_(frame.get_codec()),
   frame_number_(frame.get_real_frame_index())
{
}


vidl_frame_resource::~vidl_frame_resource()
{
}


unsigned
vidl_frame_resource::nplanes() const
{
  return codec_->get_bytes_pixel();
}


unsigned
vidl_frame_resource::ni() const
{
  return codec_->width();
}


unsigned
vidl_frame_resource::nj() const
{
  return codec_->height();
}


enum vil_pixel_format
vidl_frame_resource::pixel_format() const
{
  //FIXME find an effecient way to determine this
  return codec_->get_view(frame_number_)->pixel_format();
}


bool
vidl_frame_resource::get_property(char const *key, void * value) const
{
  if (vcl_strcmp("description", key)==0)
  {
    if (value)
      *static_cast<vcl_string*>(value) = codec_->get_description();
    return true;
  }

  return false;
}


vil_image_view_base_sptr
vidl_frame_resource::get_copy_view(unsigned x0, unsigned nx,
                                   unsigned y0, unsigned ny) const
{
  return codec_->get_view(frame_number_, x0, nx, y0, ny);
}


bool
vidl_frame_resource::put_view(const vil_image_view_base &view,
                              unsigned x0, unsigned y0)
{
  return codec_->put_view(frame_number_, view, x0, y0);
}
