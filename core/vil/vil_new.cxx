// This is core/vil/vil_new.cxx
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00

#include <cstring>
#include "vil_new.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>
#include <vil/vil_save.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_memory_image.h>
#include <vil/vil_blocked_image_facade.h>
#include <vil/vil_cached_image_resource.h>
#include <vil/vil_pyramid_image_resource.h>
#include <vil/file_formats/vil_pyramid_image_list.h>
// The first two functions really should be upgraded to create an image in
// a temporary file on disk if the sizes are large. - TODO

//: Make a new image of given format.
// If the format is not scalar, the number of planes must be 1. When you create
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                               vil_pixel_format format)
{
  assert(nplanes == 1 || vil_pixel_format_num_components(format) == 1);

  return new vil_memory_image(ni, nj, nplanes, format);
}


//: Make a new image of given format with interleaved planes.
// The format must be scalar.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource_interleaved(unsigned ni, unsigned nj,
                                                           unsigned nplanes,
                                                           vil_pixel_format format)
{
  assert(vil_pixel_format_num_components(format) == 1);

  return new vil_memory_image(ni, nj, 1, format, nplanes);
}


//: Make a new image resource that is a wrapper on an existing view's data.
// \note The output will be a shallow copy of the input, so changing the pixel values
// of one may change the pixel value of the other. Thanks to the magic of smart pointers,
// the output will remain valid even if you destroy the input. When you wrap
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource_of_view(vil_image_view_base const& view)
{
  return new vil_memory_image(view);
}

//: Make a new image, similar format to the prototype.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, vil_image_resource_sptr const& prototype)
{
  return vil_new_image_resource(ni, nj, prototype->nplanes(),
                                prototype->pixel_format());
}

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  vil_image_resource_sptr outimage = nullptr;
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto fmt : l)
  {
    if (std::strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_output_image(os, ni, nj, nplanes, format);
      if (!outimage)
        std::cerr << "vil_new: Cannot new to type [" << file_format << "]\n";
      return outimage;
    }
  }

  std::cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return nullptr;
}

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype,
                                               char const* file_format)
{
#ifdef VIL_USE_FSTREAM64
  vil_stream_fstream64* os = new vil_stream_fstream64(filename, "w");
#else //VIL_USE_FSTREAM64
  auto* os = new vil_stream_fstream(filename, "w");
#endif //VIL_USE_FSTREAM64
  return vil_new_image_resource(os,
                                ni, nj,
                                prototype->nplanes(),
                                prototype->pixel_format(),
                                file_format ? file_format : prototype->file_format());
}

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format)
{
#ifdef VIL_USE_FSTREAM64
  vil_stream_fstream64* os = new vil_stream_fstream64(filename, "w");
#else //VIL_USE_FSTREAM64
  auto* os = new vil_stream_fstream(filename, "w");
#endif //VIL_USE_FSTREAM64

  if (!file_format || !*file_format)
    file_format = vil_save_guess_file_format(filename);
  return vil_new_image_resource(os, ni, nj, nplanes, format, file_format);
}


//: Make a new vil_image_resource, writing to stream "os", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype,
                                               char const* file_format)
{
  return vil_new_image_resource(os,
                                prototype->nplanes(),
                                ni, nj,
                                prototype->pixel_format(),
                                file_format ? file_format : prototype->file_format());
}

vil_blocked_image_resource_sptr
vil_new_blocked_image_resource(vil_stream* os, unsigned ni, unsigned nj,
                               unsigned nplanes, vil_pixel_format format,
                               unsigned size_block_i, unsigned size_block_j,
                               char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  vil_blocked_image_resource_sptr outimage = nullptr;
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto fmt : l)
  {
    if (std::strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_blocked_output_image(os, ni, nj, nplanes,
                                                size_block_i, size_block_j, format);
      if (!outimage)
        std::cerr << "vil_new: Cannot new a blocked resource to type [" << file_format << "]\n";
      return outimage;
    }
  }

  std::cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return nullptr;
}

vil_blocked_image_resource_sptr
vil_new_blocked_image_resource(char const* filename, unsigned ni, unsigned nj,
                               unsigned nplanes, vil_pixel_format format,
                               unsigned size_block_i, unsigned size_block_j,
                               char const* file_format)
{
#ifdef VIL_USE_FSTREAM64
  vil_stream_fstream64* os = new vil_stream_fstream64(filename, "w");
#else //VIL_USE_FSTREAM64
  auto* os = new vil_stream_fstream(filename, "w");
#endif //VIL_USE_FSTREAM64
  return vil_new_blocked_image_resource(os, ni, nj, nplanes, format,
                                        size_block_i, size_block_j,
                                        file_format);
}

vil_blocked_image_resource_sptr
vil_new_blocked_image_facade(const vil_image_resource_sptr& src,
                             unsigned size_block_i, unsigned size_block_j)
{
  return new vil_blocked_image_facade(src, size_block_i, size_block_j);
}


vil_blocked_image_resource_sptr
vil_new_cached_image_resource(const vil_blocked_image_resource_sptr& bir,
                              const unsigned cache_size)
{
  return new vil_cached_image_resource(bir, cache_size);
}

vil_pyramid_image_resource_sptr
vil_new_pyramid_image_resource(char const* file_or_directory,
                               char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "tiff";
  vil_pyramid_image_resource_sptr outimage = nullptr;
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto fmt : l)
  {
    if (std::strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_pyramid_output_image(file_or_directory);
      if (!outimage)
        std::cerr << "vil_new: Cannot new a pyramid resource to type [" << file_format << "]\n";
      return outimage;
    }
  }
  std::cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return nullptr;
}

vil_pyramid_image_resource_sptr
  vil_new_pyramid_image_from_base(char const* filename,
                                  vil_image_resource_sptr const& base_image,
                                  unsigned nlevels,
                                  char const* file_format,
                                  char const* temp_dir)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "tiff";
  vil_pyramid_image_resource_sptr outimage = nullptr;
  std::list<vil_file_format*>& l = vil_file_format::all();
  for (auto fmt : l)
  {
    if (std::strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_pyramid_image_from_base(filename,
                                                   base_image,
                                                   nlevels,
                                                   temp_dir);
      if (!outimage)
        std::cerr << "vil_new: Cannot new a pyramid resource to type [" << file_format << "]\n";
      return outimage;
    }
  }
  std::cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return nullptr;
}

//for now there is only one directory based pyramid format
vil_pyramid_image_resource_sptr
  vil_new_pyramid_image_list_from_base(char const* directory,
                                       vil_image_resource_sptr const& base_image,
                                       unsigned nlevels,
                                       bool copy_base,
                                       char const* level_file_format,
                                       char const* filename)
{
  vil_pyramid_image_list_format vpilf;
  return vpilf.make_pyramid_image_from_base(directory, base_image, nlevels,
                                            copy_base, level_file_format,
                                            filename);
}

//: Create a shallow copy of an image and wrap it in a vil_image_view_base_sptr
// \note vil_image_view_base_sptr almost certainly doesn't behave as
// you would expect, and this function should really only be used by experts.
vil_image_view_base_sptr vil_new_image_view_base_sptr(const vil_image_view_base& src)
{
  vil_image_view_base_sptr dest;
  switch (vil_pixel_format_component_format(src.pixel_format()))
  {
#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define macro( F , T ) \
   case F: { \
      dest = new vil_image_view<T>(src); \
    break; }
   macro(VIL_PIXEL_FORMAT_BYTE, vxl_byte )
   macro(VIL_PIXEL_FORMAT_SBYTE , vxl_sbyte )
#if VXL_HAS_INT_64
   macro(VIL_PIXEL_FORMAT_UINT_64 , vxl_uint_64 )
   macro(VIL_PIXEL_FORMAT_INT_64 , vxl_int_64 )
#endif
   macro(VIL_PIXEL_FORMAT_UINT_32 , vxl_uint_32 )
   macro(VIL_PIXEL_FORMAT_INT_32 , vxl_int_32 )
   macro(VIL_PIXEL_FORMAT_UINT_16 , vxl_uint_16 )
   macro(VIL_PIXEL_FORMAT_INT_16 , vxl_int_16 )
   macro(VIL_PIXEL_FORMAT_FLOAT , float )
   macro(VIL_PIXEL_FORMAT_DOUBLE , double )
   macro(VIL_PIXEL_FORMAT_BOOL , bool )
#undef macro
#endif // DOXYGEN_SHOULD_SKIP_THIS
   default: /* do nothing */;
  }
  return dest;
}

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
#include <windows.h>

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               wchar_t const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = L"pnm";

  constexpr unsigned int size = 200;
  char fmt_buffer[size];  // should be enough
  BOOL useless;
  // ret indicates the number of characters successfully converted
  const int ret = WideCharToMultiByte(CP_ACP, 0, file_format, int(wcslen(file_format)), fmt_buffer, size, 0, &useless );
  fmt_buffer[ret] = '\0';
  if (!ret)   return 0;

  return vil_new_image_resource(os, ni, nj, nplanes, format, fmt_buffer);
}

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(wchar_t const* filename,
                                               unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype,
                                               wchar_t const* file_format)
{
#ifdef VIL_USE_FSTREAM64
  vil_stream_fstream64* os = new vil_stream_fstream64(filename, "w");
#else //VIL_USE_FSTREAM64
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
#endif //VIL_USE_FSTREAM64

  constexpr unsigned int size = 200;  // should be enough
  wchar_t tag_buffer[size];
  if ( !file_format )
  {
    char const* tag = prototype->file_format();
    const int ret = MultiByteToWideChar(CP_ACP, 0, tag, std::strlen(tag), tag_buffer, size);
    assert(ret);
    file_format = tag_buffer;  // use the file format of the given resource
  }

  return vil_new_image_resource(os,
                                ni, nj,
                                prototype->nplanes(),
                                prototype->pixel_format(),
                                file_format );
}

//: Make a new image.
// \relatesalso vil_image_resource
vil_image_resource_sptr vil_new_image_resource(wchar_t const* filename,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               wchar_t const* file_format)
{
#ifdef VIL_USE_FSTREAM64
  vil_stream_fstream64* os = new vil_stream_fstream64(filename, "w");
#else //VIL_USE_FSTREAM64
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
#endif //VIL_USE_FSTREAM64

  if (!file_format || !*file_format)
    file_format = vil_save_guess_file_format(filename);
  return vil_new_image_resource(os, ni, nj, nplanes, format, file_format);
}

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T
