// This is mul/vil3d/vil3d_new.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author Ian Scott 4 Mar 2003

#include "vil3d_new.h"

#include <vcl_cstring.h>

#include <vil3d/vil3d_file_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_memory_image.h>


// The first two functions really should be upgraded to create an image in
// a temporary file on disk if the sizes are large.

//: Make a new image of given format.
vil3d_image_resource_sptr vil3d_new_image_resource(unsigned ni, unsigned nj,
                                                   unsigned nk, unsigned nplanes,
                                                   vil_pixel_format format)
{
  return new vil3d_memory_image(ni, nj, nk, nplanes, format);
}


vil3d_image_resource_sptr vil3d_new_image_resource(const char* name,
                                                   unsigned ni,
                                                   unsigned nj,
                                                   unsigned nk,
                                                   unsigned nplanes,
                                                   vil_pixel_format format,
                                                   char const* file_type)
{
  if (!file_type) // avoid segfault in strcmp()
    file_type = "gipl";

  vil3d_image_resource_sptr outimage = 0;
  for (unsigned i=0; i < vil3d_file_format::n_formats(); ++i)
  {
    const vil3d_file_format& fmt = vil3d_file_format::format(i);
    if (vcl_strcmp(fmt.tag(), file_type) == 0) {
      outimage = fmt.make_output_image(name, ni, nj, nk, nplanes, format);
      if (!outimage)
        vcl_cerr << "vil3d_new: Cannot new to type [" << file_type << "]\n";
      return outimage;
    }
  }

  vcl_cerr << "vil3d_new: Unknown file type [" << file_type << "]\n";
  vcl_cerr << "Known file types are: ";
  for (unsigned i=0; i < vil3d_file_format::n_formats(); ++i)
  {
    if (i) vcl_cerr << ',';
    vcl_cerr << vil3d_file_format::format(i).tag() ;
  }
  vcl_cerr << '.' << vcl_endl;

  return 0;
}
