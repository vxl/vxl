// This is mul/vil2/vil2_save.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil2_save.h"

#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

#include <vil/vil_open.h>
#include <vil2/vil2_new.h>
#include <vil2/vil2_pixel_format.h>
#include <vil2/vil2_image_data.h>
#include <vil2/vil2_image_view_functions.h>


//: Send vil2_image to disk.
bool vil2_save(const vil2_image_view_base &im, char const* filename, char const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
  if (!os->ok()) {
    vcl_cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil2_image_data_sptr out = vil2_new_image_data(os, im.ni(), im.nj(),
    im.nplanes() * vil2_pixel_format_num_components(im.pixel_format()),
    im.pixel_format(), file_format);
  if (!out) {
    vcl_cerr << __FILE__ ": (vil2_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }
  switch (vil2_pixel_format_component_format(im.pixel_format()))
  {
  case VIL2_PIXEL_FORMAT_BYTE:
    return out->put_view(vil2_image_view<vxl_byte>(im),0,0);
  case VIL2_PIXEL_FORMAT_UINT_16:
    return out->put_view(vil2_image_view<vxl_uint_16>(im),0,0);
  case VIL2_PIXEL_FORMAT_INT_16:
    return out->put_view(vil2_image_view<vxl_int_16>(im),0,0);
  case VIL2_PIXEL_FORMAT_UINT_32:
    return out->put_view(vil2_image_view<vxl_uint_32>(im),0,0);
  case VIL2_PIXEL_FORMAT_INT_32:
    return out->put_view(vil2_image_view<vxl_int_32>(im),0,0);
  case VIL2_PIXEL_FORMAT_FLOAT:
    return out->put_view(vil2_image_view<float>(im),0,0);
  case VIL2_PIXEL_FORMAT_BOOL:
    return out->put_view(vil2_image_view<bool>(im),0,0);
#ifdef VIL2_TO_BE_FIXED
  case VIL2_PIXEL_FORMAT_SBYTE:
    return out->put_view(vil2_image_view<vxl_sbyte>(im),0,0);
  case VIL2_PIXEL_FORMAT_DOUBLE:
    return out->put_view(vil2_image_view<double>(im),0,0);
#endif
  default:
    return out->put_view(im, 0, 0);
  }

#ifdef VIL2_TO_BE_FIXED
  bool top_first, bgr;
  if (out.get_property(vil2_property_top_row_first, &top_first) && !top_first)
    im = vil2_flipud(i);
  if (i.components() == 3 && out.get_property(vil2_property_component_order_is_BGR, &bgr) && bgr)
    im = vil2_flip_components(i);
#endif
}


static
char const *guess_file_format(char const* filename)
{
  char const *file_format = 0;

  // find last "."
  char const *dot = vcl_strrchr(filename, '.');
  if (!dot) {
    // filename doesn't end in ".anything"
    vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
    file_format = "pnm";
  }
  else {
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if (!vcl_strcmp(dot, "." #ext)) file_format = #fmt
    macro(bmp, bmp);
    macro(pbm, pnm);
    macro(pgm, pnm);
    macro(ppm, pnm);
    macro(pnm, pnm);
    macro(jpg, jpeg);
    macro(tif, tiff);
    macro(mit, mit);
    macro(gif, gif);
    macro(png, png);
#undef macro
    else {
      //file_format = dot+1; // hope it works.
      vcl_cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
      file_format = "pnm";
    }
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil2_save(const vil2_image_view_base & i, char const* filename)
{
  return vil2_save(i, filename, guess_file_format(filename));
}

