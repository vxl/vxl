// This is core/vil/vil_save.cxx
//:
// \file
//
// \verbatim
//  Modifications
//   23 Oct.2003 - Peter Vanroose - Added support for 64-bit int pixels
// \endverbatim

#include <cctype>
#include <cstring>
#include <string>
#include <iostream>
#include "vil_save.h"

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


//: Send vil_image to disk.
bool vil_save(const vil_image_view_base &im, char const* filename, char const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
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


char const *vil_save_guess_file_format(char const* filename)
{
  char const *file_format = "pnm"; // default file format

  // find last "."
  char const *dot = std::strrchr(filename, '.');
  if (!dot) {
    // filename doesn't end in ".anything"
    std::cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
    file_format = "pnm";
  }
  else {
    std::string ext_lower_case(dot);  // make a copy to convert the extension to lower case
    for (char & i : ext_lower_case)
      i = (char)std::tolower(i);
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if ( ext_lower_case == "." #ext ) file_format = #fmt
    macro(tiff, tiff);
    macro(tif, tiff);
    macro(png, png);
    macro(bmp, bmp);
    macro(pbm, pnm);
    macro(pgm, pnm);
    macro(ppm, pnm);
    macro(pnm, pnm);
    macro(jpg, jpeg);
    macro(jpeg, jpeg);
    macro(iris, iris);
    macro(rgb, iris);
    macro(viff, viff);
    macro(mit, mit);
    macro(v2i, v2i);
    macro(sgi, sgi);
#undef macro
    else
      std::cerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil_save(const vil_image_view_base & i, char const* filename)
{
  return vil_save(i, filename, vil_save_guess_file_format(filename));
}

//: Send vil_image to disk.
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename,
                             char const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
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
bool vil_save_image_resource(const vil_image_resource_sptr &ir, char const* filename)
{
  return vil_save_image_resource(ir, filename, vil_save_guess_file_format(filename));
}



#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//  --------------------------------------------------------------------------------
//  Windows' wchar_t overloading version
//
//


//: Send vil_image to disk.
bool vil_save(const vil_image_view_base &im, wchar_t const* filename, wchar_t const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
  if (!os || !os->ok()) {
    std::wcerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil_image_resource_sptr out = vil_new_image_resource(os, im.ni(), im.nj(),
                                                       im.nplanes() * vil_pixel_format_num_components(im.pixel_format()),
                                                       vil_pixel_format_component_format(im.pixel_format()), file_format);
  if (!out) {
    std::wcerr << __FILE__ ": (vil_save) Cannot save to type [" << file_format << "]\n";
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


wchar_t const *vil_save_guess_file_format(wchar_t const* filename)
{
  wchar_t const *file_format = L"pnm"; // default file format

  // find last "."
  wchar_t const *dot = wcsrchr(filename, L'.');
  if (!dot) {
    // filename doesn't end in ".anything"
    std::wcerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
    file_format = L"pnm";
  }
  else {
    std::wstring ext_lower_case(dot);  // make a copy to convert the extension to lower case
    for(unsigned int i=0; i<ext_lower_case.size(); ++i)
      ext_lower_case[i] = towlower(ext_lower_case[i]);
    // translate common extensions into known file formats.
    if (false) { }
#define macro(ext, fmt) else if ( ext_lower_case == L"." L#ext ) file_format = L#fmt
    macro(tiff, tiff);
    macro(tif, tiff);
    macro(png, png);
    macro(bmp, bmp);
    macro(pbm, pnm);
    macro(pgm, pnm);
    macro(ppm, pnm);
    macro(pnm, pnm);
    macro(jpg, jpeg);
    macro(jpeg, jpeg);
    macro(iris, iris);
    macro(rgb, iris);
    macro(viff, viff);
    macro(mit, mit);
    macro(v2i, v2i);
#undef macro
    else
      std::wcerr << __FILE__ ": assuming pnm format for \'" << filename << "\'\n";
  }

  return file_format;
}

//: save to file, deducing format from filename.
bool vil_save(const vil_image_view_base & i, wchar_t const* filename)
{
  return vil_save(i, filename, vil_save_guess_file_format(filename));
}

//: Send vil_image to disk.
bool vil_save_image_resource(const vil_image_resource_sptr &ir, wchar_t const* filename,
                             wchar_t const* file_format)
{
  vil_stream* os = vil_open(filename, "w");
  if (!os || !os->ok()) {
    std::wcerr << __FILE__ ": Invalid stream for \"" << filename << "\"\n";
    return false;
  }
  vil_image_resource_sptr out = vil_new_image_resource(os, ir->ni(), ir->nj(),
                                                       ir->nplanes(),
                                                       ir->pixel_format(), file_format);
  if (!out) {
    std::wcerr << __FILE__ ": (vil_save) Cannot save to type [" << file_format << "]\n";
    return false;
  }
  return vil_copy_deep(ir, out);
}

//: save to file, deducing format from filename.
bool vil_save_image_resource(const vil_image_resource_sptr &ir, wchar_t const* filename)
{
  return vil_save_image_resource(ir, filename, vil_save_guess_file_format(filename));
}

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T
