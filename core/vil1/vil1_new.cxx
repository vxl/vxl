// This is core/vil1/vil1_new.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00
//
//-----------------------------------------------------------------------------

#include "vil1_new.h"

#include <vcl_cstring.h>
#include <vcl_cassert.h>

#include <vil1/vil1_file_format.h>
#include <vil1/vil1_stream_fstream.h>
#include <vil1/vil1_image_impl.h>
#include <vil1/vil1_image.h>
#include <vil1/vil1_memory_image_of.h>

vil1_image vil1_new(int width, int height, vil1_image const& prototype)
{
  switch (vil1_pixel_format(prototype)) {
  case VIL1_BYTE: return vil1_memory_image_of<unsigned char>(width, height);
  default:
    assert(!"vil1_new");
    return 0;
  }
}


vil1_image vil1_new(vil1_stream* os,
                    int planes,
                    int width,
                    int height,
                    int components,
                    int bits_per_component,
                    vil1_component_format format,
                    char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  for (vil1_file_format** p = vil1_file_format::all(); *p; ++p) {
    vil1_file_format* fmt = *p;
    if (vcl_strcmp(fmt->tag(), file_format) == 0) {
      vil1_image_impl* outimage = fmt->make_output_image(os, planes, width, height, components, bits_per_component, format);
      if (outimage == 0)
        vcl_cerr << "vil1_new: Unknown cannot new to type [" << file_format << "]\n";
      return outimage;
    }
  }

  vcl_cerr << "vil1_new: Unknown file type [" << file_format << "]\n";
  return 0;
}

//: Make a new vil1_image_impl, writing to stream "os", size "w" x "h", copying pixel format etc from "prototype".
vil1_image vil1_new(vil1_stream* os,
                    int width, int height,
                    vil1_image const& prototype,
                    char const* file_format)
{
  return vil1_new(os,
                  prototype.planes(),
                  width,
                  height,
                  prototype.components(),
                  prototype.bits_per_component(),
                  prototype.component_format(),
                  file_format ? file_format : prototype.file_format());
}

//: Make a new vil1_image_impl, writing to file "filename", size "w" x "h", copying pixel format etc from "prototype".
vil1_image vil1_new(char const* filename,
                    int width, int height,
                    vil1_image const& prototype,
                    char const* file_format)
{
  vil1_stream_fstream* os = new vil1_stream_fstream(filename, "w");
  return vil1_new(os,
                  prototype.planes(),
                  width,
                  height,
                  prototype.components(),
                  prototype.bits_per_component(),
                  prototype.component_format(),
                  file_format ? file_format : prototype.file_format());
}
