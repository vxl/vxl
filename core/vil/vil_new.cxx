// This is mul/vil2/vil2_new.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00
//
//\verbatim
//  Modifications
//     25 Sep 2002 Ian Scott - convert to vil2.
//\endverbatim


#include "vil2_new.h"

#include <vcl_cstring.h>
#include <vcl_cassert.h>

#include <vil2/vil2_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil2/vil2_image_data.h>
#include <vil2/vil2_memory_image.h>

vil2_image_data_sptr vil2_new_image_data(unsigned ni, unsigned nj, vil2_image_data_sptr const& prototype)
{
  return new vil2_memory_image(ni, nj, prototype->nplanes(),
    prototype->pixel_format());
}


vil2_image_data_sptr vil2_new_image_data(vil_stream* os,
                  unsigned ni,
                  unsigned nj,
                  unsigned nplanes,
                  vil2_pixel_format format,
                  char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  vil2_image_data_sptr outimage = 0;
  for (vil2_file_format** p = vil2_file_format::all(); *p; ++p) {
    vil2_file_format* fmt = *p;
    if (vcl_strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_output_image(os, ni, nj, nplanes, format);
      if (!outimage) {
        vcl_cerr << "vil2_new: Unknown cannot new to type [" << file_format << "]\n";
      }
      return outimage;
    }
  }

  vcl_cerr << "vil2_new: Unknown file type [" << file_format << "]\n";
  return 0;
}

//: Make a new vil2_image_impl, writing to stream "os", size ni x nj, copying pixel format etc from "prototype".
vil2_image_data_sptr vil2_new_image_data(vil_stream* os,
                  unsigned ni, unsigned nj,
                  vil2_image_data_sptr const& prototype,
                  char const* file_format)
{
  return vil2_new_image_data(os,
                 prototype->nplanes(),
                 ni, nj,
                 prototype->pixel_format(),
                 file_format ? file_format : prototype->file_format());
}

//: Make a new vil2_image_impl, writing to file "filename", size "w" x "h", copying pixel format etc from "prototype".
vil2_image_data_sptr vil2_new_image_data(char const* filename,
                  unsigned ni, unsigned nj,
                  vil2_image_data_sptr const& prototype,
                  char const* file_format)
{
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
  return vil2_new_image_data(os,
                 ni, nj,
                 prototype->nplanes(),
                 prototype->pixel_format(),
                 file_format ? file_format : prototype->file_format());
}
