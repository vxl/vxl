// This is brl/bbas/bhdfs/bhdfs_vil_save.cxx
#include <iostream>
#include <cctype>
#include <cstring>
#include <string>
#include "bhdfs_vil_save.h"
//:
// \file
//
// \verbatim
//  Modifications
//
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vxl_config.h> // for vxl_byte

#include <vil/vil_open.h>
#include <vil/vil_new.h>
#include <vil/vil_copy.h>
#include <vil/vil_pixel_format.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

#include "bhdfs_vil_stream.h"


//: Send vil_image to disk.
bool bhdfs_vil_save(const vil_image_view_base &im, char const* filename, char const* file_format)
{
  vil_stream* os = new bhdfs_vil_stream(filename, "w");
  if (!os || !os->ok()) {
    std::cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil_image_resource_sptr out = vil_new_image_resource(os, im.ni(), im.nj(),
                                                       im.nplanes() * vil_pixel_format_num_components(im.pixel_format()),
                                                       vil_pixel_format_component_format(im.pixel_format()), file_format);
  if (!out) {
    std::cerr << __FILE__ ": (vil_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }

  // Use smart copy constructor to convert multi-component images
  // into multi-plane ones.
  switch (vil_pixel_format_component_format(im.pixel_format()))
  {
  case VIL_PIXEL_FORMAT_BYTE:
    return out->put_view(vil_image_view<vxl_byte>(im),0,0);
  case VIL_PIXEL_FORMAT_UINT_16:
    return out->put_view(vil_image_view<vxl_uint_16>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_16:
    return out->put_view(vil_image_view<vxl_int_16>(im),0,0);
  case VIL_PIXEL_FORMAT_UINT_32:
    return out->put_view(vil_image_view<vxl_uint_32>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_32:
    return out->put_view(vil_image_view<vxl_int_32>(im),0,0);
#if VXL_HAS_INT_64
  case VIL_PIXEL_FORMAT_UINT_64:
    return out->put_view(vil_image_view<vxl_uint_64>(im),0,0);
  case VIL_PIXEL_FORMAT_INT_64:
    return out->put_view(vil_image_view<vxl_int_64>(im),0,0);
#endif
  case VIL_PIXEL_FORMAT_FLOAT:
    return out->put_view(vil_image_view<float>(im),0,0);
  case VIL_PIXEL_FORMAT_BOOL:
    return out->put_view(vil_image_view<bool>(im),0,0);
  case VIL_PIXEL_FORMAT_SBYTE:
    return out->put_view(vil_image_view<vxl_sbyte>(im),0,0);
  case VIL_PIXEL_FORMAT_DOUBLE:
    return out->put_view(vil_image_view<double>(im),0,0);
  default:
    // In case any one has an odd pixel format that actually works with this file_format.
    return out->put_view(im, 0, 0);
  }
}

//: save to file, deducing format from filename.
bool bhdfs_vil_save(const vil_image_view_base & i, char const* filename)
{
  return bhdfs_vil_save(i, filename, vil_save_guess_file_format(filename));
}

//: Send vil_image to disk.
bool bhdfs_vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename,
                                   char const* file_format)
{
  //vil_stream* os = vil_open(filename, "w");
  vil_stream* os = new bhdfs_vil_stream(filename, "w");
  if (!os || !os->ok()) {
    std::cerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil_image_resource_sptr out = vil_new_image_resource(os, ir->ni(), ir->nj(),
                                                       ir->nplanes(),
                                                       ir->pixel_format(), file_format);
  if (!out) {
    std::cerr << __FILE__ ": (vil_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }
  return vil_copy_deep(ir, out);
}

//: save to file, deducing format from filename.
bool bhdfs_vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename)
{
  return bhdfs_vil_save_image_resource(ir, filename, vil_save_guess_file_format(filename));
}
