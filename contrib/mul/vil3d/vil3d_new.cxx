// This is mul/vil3d/vil3d_new.cxx
//:
// \file
// \author Ian Scott 4 Mar 2003

#include <iostream>
#include <cstring>
#include "vil3d_new.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil3d/vil3d_file_format.h>
#include <vil3d/vil3d_image_resource.h>
#include <vil3d/vil3d_memory_image.h>
#include <vil3d/vil3d_save.h>


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
    file_type = vil3d_save_guess_file_format(name);



  vil3d_image_resource_sptr outimage = nullptr;
  for (unsigned i=0; i < vil3d_file_format::n_formats(); ++i)
  {
    const vil3d_file_format& fmt = vil3d_file_format::format(i);
    if (std::strcmp(fmt.tag(), file_type) == 0) {
      outimage = fmt.make_output_image(name, ni, nj, nk, nplanes, format);
      if (!outimage)
        std::cerr << "vil3d_new: Cannot new to type [" << file_type << "]\n";
      return outimage;
    }
  }

  std::cerr << "vil3d_new: Unknown file type [" << file_type << "]\n";
  std::cerr << "Known file types are: ";
  for (unsigned i=0; i < vil3d_file_format::n_formats(); ++i)
  {
    if (i) std::cerr << ',';
    std::cerr << vil3d_file_format::format(i).tag() ;
  }
  std::cerr << '.' << std::endl;

  return nullptr;
}
