//:
// \file
// \brief Functions to read an image from a file
// \author Ian Scott

#include "vil3d_load.h"
#include <vil3d/vil3d_file_format.h>


vil3d_image_resource_sptr vil3d_load_image_resource(char const* filename)
{
  for (unsigned i=0;i<vil3d_file_format::n_formats();++i)
  {
    vil3d_image_resource_sptr im =
      vil3d_file_format::format(i).make_input_image(filename);
    if (im) return im;
  }
  vcl_cerr << "WARNING vil3d_load unable to load " << filename <<
      "\n Tried ";
  for (unsigned i=0;i<vil3d_file_format::n_formats();++i)
    vcl_cerr << vil3d_file_format::format(i).tag() << ' ';
  vcl_cerr << vcl_endl; 
  return 0;
}


//: Convenience function for loading an image into an image view.
vil3d_image_view_base_sptr vil3d_load(const char *file)
{
  vil3d_image_resource_sptr data = vil3d_load_image_resource(file);
  if (!data) return 0;
  return data -> get_view(0, data->ni(), 0, data->nj(), 0, data->nk());
}
