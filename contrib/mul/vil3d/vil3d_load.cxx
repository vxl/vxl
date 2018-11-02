#include <iostream>
#include <sstream>
#include "vil3d_load.h"
//:
// \file
// \brief Functions to read an image from a file
// \author Ian Scott

#include <vil/vil_exception.h>
#include <vil3d/vil3d_file_format.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

vil3d_image_resource_sptr vil3d_load_image_resource(char const* filename)
{
  for (unsigned i=0;i<vil3d_file_format::n_formats();++i)
  {
    vil3d_image_resource_sptr im =
      vil3d_file_format::format(i).make_input_image(filename);
    if (im) return im;
  }
  std::ostringstream ss;
  for (unsigned i=0;i+1<vil3d_file_format::n_formats();++i)
    ss << vil3d_file_format::format(i).tag() << ' ';
  if (vil3d_file_format::n_formats() > 1)
    ss << "or ";
  if (vil3d_file_format::n_formats())
    ss << vil3d_file_format::format(vil3d_file_format::n_formats()-1).tag();
  vil_exception_warning(vil_exception_image_io("vil3d_load_image_resource",
                         ss.str() , filename, "Unable to find a suitable image loader." ) );
  return nullptr;
}


//: Convenience function for loading an image into an image view.
vil3d_image_view_base_sptr vil3d_load(const char *file)
{
  vil3d_image_resource_sptr data = vil3d_load_image_resource(file);
  if (!data) return nullptr;
  return data -> get_view(0, data->ni(), 0, data->nj(), 0, data->nk());
}
