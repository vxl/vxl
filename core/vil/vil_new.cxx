// This is core/vil/vil_new.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Andrew W. Fitzgibbon, Oxford RRG
// \date   16 Feb 00

#include "vil_new.h"

#include <vcl_cstring.h>
#include <vcl_cassert.h>

#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_memory_image.h>


// The first two functions really should be upgraded to create an image in
// a temporary file on disk if the sizes are large. - TODO

//: Make a new image of given format.
// If the format is not scalar, the number of planes must be 1. When you create
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, unsigned nplanes,
                                               vil_pixel_format format)
{
  assert(nplanes == 1 || vil_pixel_format_num_components(format) == 1);

  return new vil_memory_image(ni, nj, nplanes, format);
}


//: Make a new image resource that is a wrapper on an existing view's data.
// \note The output will be a shallow copy of the input, so changing the pixel values
// of one may change the pixel value of the other. Thanks to the magic of smart pointers,
// the output will remain valid even if you destroy the input. When you wrap
// a multi-component image in this way, the vil_image_resource API will treat
// it as a scalar pixel image with multiple planes. (This doesn't affect the
// underlying data storage.)
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource_of_view(vil_image_view_base const& view)
{
  return new vil_memory_image(view);
}

//: Make a new image, similar format to the prototype.
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(unsigned ni, unsigned nj, vil_image_resource_sptr const& prototype)
{
  return vil_new_image_resource(ni, nj, prototype->nplanes(),
                                prototype->pixel_format());
}

//: Make a new image.
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(vil_stream* os,
                                               unsigned ni, unsigned nj,
                                               unsigned nplanes,
                                               vil_pixel_format format,
                                               char const* file_format)
{
  if (!file_format) // avoid segfault in strcmp()
    file_format = "pnm";

  vil_image_resource_sptr outimage = 0;
  for (vil_file_format** p = vil_file_format::all(); *p; ++p)
  {
    vil_file_format* fmt = *p;
    if (vcl_strcmp(fmt->tag(), file_format) == 0) {
      outimage = fmt->make_output_image(os, ni, nj, nplanes, format);
      if (!outimage)
        vcl_cerr << "vil_new: Cannot new to type [" << file_format << "]\n";
      return outimage;
    }
  }

  vcl_cerr << "vil_new: Unknown file type [" << file_format << "]\n";
  return 0;
}

//: Make a new vil_image_resource, writing to file "filename", size ni x nj, copying pixel format etc from "prototype".
// \relates vil_image_resource
vil_image_resource_sptr vil_new_image_resource(char const* filename,
                                               unsigned ni, unsigned nj,
                                               vil_image_resource_sptr const& prototype,
                                               char const* file_format)
{
  vil_stream_fstream* os = new vil_stream_fstream(filename, "w");
  return vil_new_image_resource(os,
                                ni, nj,
                                prototype->nplanes(),
                                prototype->pixel_format(),
                                file_format ? file_format : prototype->file_format());
}

//: Make a new vil_image_resource, writing to stream "os", size ni x nj, copying pixel format etc from "prototype".
// \relates vil_image_resource
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
