

#include <vcl_cassert.h>

#include "oxp_vidl_mpeg_codec.h"

// leave non-inline so compiler plonks vtbl nicely

const char* 
oxp_vidl_mpeg_codec::type()
{
  return "MPEG";
}

vidl_vil1_codec_sptr
oxp_vidl_mpeg_codec::load(const char* fname, char mode) 
{
  if (!p.load(fname, mode))
    return 0;

  // random vidl stuff
  set_format('L');
  set_image_class('C');
  set_name(fname);
  set_description(fname);

  set_bits_pixel(24);
  set_size_z(1);
  set_size_t(1);
  set_height(p.get_height());
  set_width(p.get_width());
  set_number_frames(299999);

  return this;
}

bool
oxp_vidl_mpeg_codec::save(vidl_vil1_movie* movie, const char* fname)
{
  assert(0);
  return false;
}
