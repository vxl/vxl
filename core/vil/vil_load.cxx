// This is core/vil/vil_load.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil_load.h"
#include <vcl_iostream.h>
#include <vil/vil_open.h>
#include <vil/vil_new.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_resource_plugin.h>
#include <vil/vil_image_view.h>

vil_image_resource_sptr vil_load_image_resource_raw(vil_stream *is)
{
  for (vil_file_format** p = vil_file_format::all(); *p; ++p) {
#if 0 // debugging
    vcl_cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";
#endif
    is->seek(0);
    vil_image_resource_sptr im = (*p)->make_input_image(is);
    if (im)
      return im;
  }

  // failed.
  vcl_cerr << __FILE__ ": Unable to load image;\ntried";
  for (vil_file_format** p = vil_file_format::all(); *p; ++p)
    // 'flush' in case of segfault next time through loop. Else, we
    // will not see those printed tags still in the stream buffer.
    vcl_cerr << " \'" << (*p)->tag() << "\'" << vcl_flush;
  vcl_cerr << vcl_endl;

  return 0;
}

vil_image_resource_sptr vil_load_image_resource_raw(char const* filename)
{
  vil_stream *is = vil_open(filename, "r");
  if (is)
    return vil_load_image_resource_raw(is);
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
    return vil_image_resource_sptr(0);
  }
}

vil_image_resource_sptr vil_load_image_resource(char const* filename)
{
  vil_image_resource_sptr im = vil_load_image_resource_plugin(filename);
  if (!im)
    im=vil_load_image_resource_raw(filename);
  return im;
}


vil_image_resource_sptr vil_load_image_resource_plugin(char const* filename)
{
  vil_image_resource_plugin im_resource_plugin;
  if (im_resource_plugin.can_be_loaded(filename))
  {
    vil_image_view_base* img=new vil_image_view<vxl_byte>(640,480,3);
    vil_image_resource_sptr im;
    vil_image_view_base_sptr im_view(img);
    if (im_resource_plugin.load_the_image(im_view,filename))
    {
      im=vil_new_image_resource(im_view->ni(),im_view->nj(),
                                im_view->nplanes(),im_view->pixel_format());
      if (im->put_view((const vil_image_view_base&)*im_view,0,0))
        return im;
    }
  }
  return vil_image_resource_sptr(0);
}


//: Convenience function for loading an image into an image view.
vil_image_view_base_sptr vil_load(const char *file)
{
  vil_image_resource_sptr data = vil_load_image_resource(file);
  if (!data) return 0;
  return data -> get_view();
}
