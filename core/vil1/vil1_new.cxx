// This is vxl/vil/vil_new.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil_new.h"

#include <vcl_cstring.h>
#include <vcl_cassert.h>

#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_impl.h>
#include <vil/vil_image.h>
#include <vil/vil_memory_image_of.h>

vil_image vil_new(int width, int height, vil_image const& prototype)
{
  switch (vil_pixel_format(prototype)) {
  case VIL_BYTE: return vil_memory_image_of<unsigned char>(width, height);
  default:
    assert(!"vil_new");
    return 0;
  }
}


vil_image vil_new(vil_stream* os,
                  int planes,
                  int width,
                  int height,
                  int components,
                  int bits_per_component,
                  vil_component_format format,
                  char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  vil_image_impl* outimage = 0;
  for (vil_file_format** p = vil_file_format::all(); *p; ++p) {
    vil_file_format* fmt = *p;
    if (vcl_strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_output_image(os, planes, width, height, components, bits_per_component, format);
      if (outimage == 0) {
        vcl_cerr << "vil_new: Unknown cannot new to type [" << file_format << "]\n";
      }
      return outimage;
    }
  }

  vcl_cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return 0;
}

//: Make a new vil_image_impl, writing to stream "os", size "w" x "h", copying pixel format etc from "prototype".
vil_image vil_new(vil_stream* os,
                  int width, int height,
                  vil_image const& prototype,
                  char const* file_format)
{
  return vil_new(os,
                 prototype.planes(),
                 width,
                 height,
                 prototype.components(),
                 prototype.bits_per_component(),
                 prototype.component_format(),
                 file_format ? file_format : prototype.file_format());
}

//: Make a new vil_image_impl, writing to file "filename", size "w" x "h", copying pixel format etc from "prototype".
vil_image vil_new(char const* filename,
                  int width, int height,
                  vil_image const& prototype,
                  char const* file_format)
{
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
  return vil_new(os,
                 prototype.planes(),
                 width,
                 height,
                 prototype.components(),
                 prototype.bits_per_component(),
                 prototype.component_format(),
                 file_format ? file_format : prototype.file_format());
}
