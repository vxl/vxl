// This is mul/vil3d/vil3d_save.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
//


#include "vil3d_save.h"

#include <vcl_cstring.h>
#include <vcl_iostream.h>
#include <vxl_config.h> // for vxl_byte

#include <vil3d/vil3d_new.h>
#include <vil3d/vil3d_copy.h>
#include <vil/vil_pixel_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_image_view.h>


//: Send vil3d_image to disk.
bool vil3d_save(const vil3d_image_view_base &im, char const* filename, char const* file_format)
{

  vil3d_image_resource_sptr out =
    vil3d_new_image_resource(filename, im.ni(), im.nj(), im.nk(),
                             im.nplanes() * vil_pixel_format_num_components(im.pixel_format()),
                             im.pixel_format(), file_format);
  if (!out) {
    vcl_cerr << __FILE__ ": (vil3d_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }

  // Use smart copy constructor to convert multi-component images
  // into multi-plane ones.
  switch (vil_pixel_format_component_format(im.pixel_format()))
  {
  case VIL_PIXEL_FORMAT_BYTE:
    return out->put_view(vil3d_image_view<vxl_byte>(im),0,0,0);
  case VIL_PIXEL_FORMAT_UINT_16:
    return out->put_view(vil3d_image_view<vxl_uint_16>(im),0,0,0);
  case VIL_PIXEL_FORMAT_INT_16:
    return out->put_view(vil3d_image_view<vxl_int_16>(im),0,0,0);
  case VIL_PIXEL_FORMAT_UINT_32:
    return out->put_view(vil3d_image_view<vxl_uint_32>(im),0,0,0);
  case VIL_PIXEL_FORMAT_INT_32:
    return out->put_view(vil3d_image_view<vxl_int_32>(im),0,0,0);
  case VIL_PIXEL_FORMAT_FLOAT:
    return out->put_view(vil3d_image_view<float>(im),0,0,0);
  case VIL_PIXEL_FORMAT_BOOL:
    return out->put_view(vil3d_image_view<bool>(im),0,0,0);
  case VIL_PIXEL_FORMAT_SBYTE:
    return out->put_view(vil3d_image_view<vxl_sbyte>(im),0,0,0);
  case VIL_PIXEL_FORMAT_DOUBLE:
    return out->put_view(vil3d_image_view<double>(im),0,0,0);
  default:
    // In case any one has an odd pixel format that actually works with this file_format.
    return out->put_view(im, 0, 0, 0);
  }
}

static
char const *guess_file_format(char const* filename)
{
  char const *file_format;

  // find last "."
  char const *dot = vcl_strrchr(filename, '.');
  if (!dot) {
    // filename doesn't end in ".anything"
    vcl_cerr << __FILE__ ": assuming gipl format for \'" << filename << "\'\n";
    file_format = "gipl";
  }
  else {
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if (!vcl_strcmp(dot, "." #ext)) file_format = #fmt
    macro(dcm, dicom);
#undef macro
    else
      file_format=dot+1;
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil3d_save(const vil3d_image_view_base & i, char const* filename)
{
  return vil3d_save(i, filename, guess_file_format(filename));
}

//: Send vil3d_image to disk.
bool vil3d_save_image_resource(const vil3d_image_resource_sptr &ir, char const* filename,
                             char const* file_format)
{
  vil3d_image_resource_sptr out = vil3d_new_image_resource(filename, ir->ni(), ir->nj(), ir->nk(),
                                                           ir->nplanes(),
                                                           ir->pixel_format(), file_format);
  if (!out) {
    vcl_cerr << __FILE__ ": (vil3d_save) Cannot save type [" << file_format << "] to ["<<filename<<"]\n";
    return false;
  }
  return vil3d_copy_deep(ir, out);
}

//: save to file, deducing format from filename.
bool vil3d_save_image_resource(const vil3d_image_resource_sptr &ir, char const* filename)
{
  return vil3d_save_image_resource(ir, filename, guess_file_format(filename));
}
