// This is core/vil2/vil2_load.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil2_load.h"
#include <vcl_iostream.h>
#include <vil2/vil2_open.h>
#include <vil2/vil2_file_format.h>
#include <vil2/vil2_stream.h>
#include <vil2/vil2_image_resource.h>
#include <vil2/vil2_image_resource_plugin.h>
#include <vil2/vil2_memory_image.h>
#include <vil2/vil2_image_view.h>

vil2_image_resource_sptr vil2_load_image_resource_raw(vil2_stream *is)
{
  for (vil2_file_format** p = vil2_file_format::all(); *p; ++p) {
#if 0 // debugging
    vcl_cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";
#endif
    is->seek(0);
    vil2_image_resource_sptr im = (*p)->make_input_image(is);
    if (im)
      return im;
  }

  // failed.
  vcl_cerr << __FILE__ ": Tried";
  for (vil2_file_format** p = vil2_file_format::all(); *p; ++p)
    // 'flush' in case of segfault next time through loop. Else, we
    // will not see those printed tags still in the stream buffer.
    vcl_cerr << " \'" << (*p)->tag() << "\'" << vcl_flush;
  vcl_cerr << vcl_endl;

  return 0;
}

vil2_image_resource_sptr vil2_load_image_resource_raw(char const* filename)
{
  vil2_stream *is = vil2_open(filename, "r");
  if (is)
    return vil2_load_image_resource_raw(is);
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
    return vil2_image_resource_sptr(0);
  }
}

vil2_image_resource_sptr vil2_load_image_resource(char const* filename)
{
  vil2_image_resource_sptr im = vil2_load_image_resource_plugin(filename);
  if (!im)
    {
    im=vil2_load_image_resource_raw(filename);
    }
  return im;
}


vil2_image_resource_sptr vil2_load_image_resource_plugin(char const* filename)
{
  vil2_image_resource_plugin im_ressource_plugin;
  if (im_ressource_plugin.can_be_loaded(filename))
    {
    vil2_image_view_base* img=new vil2_image_view<vxl_byte>(640,480,3);
    vil2_image_resource_sptr im;
    vil2_image_view_base_sptr im_view(img);
    if (im_ressource_plugin.load_the_image(im_view,filename))
      {
      im=vil2_new_image_resource(im_view->ni(),im_view->nj(),
          im_view->nplanes(),im_view->pixel_format());
      if (im->put_view((const vil2_image_view_base&)*im_view,0,0))
        {
        return im;
        }
      }
    }
  return vil2_image_resource_sptr(0);
}


//: Convenience function for loading an image into an image view.
vil2_image_view_base_sptr vil2_load(const char *file)
{
  vil2_image_resource_sptr data = vil2_load_image_resource(file);
  if (!data) return 0;
  return data -> get_view(0, data->ni(), 0, data->nj());
}
