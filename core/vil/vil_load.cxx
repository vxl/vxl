// This is mul/vil2/vil2_load.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file

#include "vil2_load.h"
#include <vcl_iostream.h>
#include <vil/vil_open.h>
#include <vil2/vil2_file_format.h>
#include <vil/vil_stream.h>
#include <vil2/vil2_property.h>
#include <vil2/vil2_image_resource.h>


vil2_image_resource_sptr vil2_load_image_resource_raw(vil_stream *is)
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
  vil_stream *is = vil_open(filename, "r");
  if (is)
    return vil2_load_image_resource_raw(is);
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
    return vil2_image_resource_sptr(0);
  }
}

vil2_image_resource_sptr vil2_load_image_resource(char const* filename)
{
  vil2_image_resource_sptr im = vil2_load_image_resource_raw(filename);
  return im;
}


//: Convenience function for loading an image into an image view.
vil2_image_view_base_sptr vil2_load(const char *file)
{
  vil2_image_resource_sptr data = vil2_load_image_resource(file);
  if (!data) return 0;
  return data -> get_view(0, data->ni(), 0, data->nj());
}
