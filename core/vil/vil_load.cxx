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
#ifdef VIL2_TO_BE_FIXED
#include <vil/vil_flipud.h>
#include <vil/vil_flip_components.h>
#endif
#include <vil2/vil2_image_data.h>


vil2_image_data_sptr vil2_load_image_data_raw(vil_stream *is)
{
  for (vil2_file_format** p = vil2_file_format::all(); *p; ++p) {
#if 0 // debugging
    vcl_cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'\n";
#endif
    is->seek(0);
    vil2_image_data_sptr im = (*p)->make_input_image(is);
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

vil2_image_data_sptr vil2_load_image_data_raw(char const* filename)
{
  vil_stream *is = vil_open(filename, "r");
  if (is)
    return vil2_load_image_data_raw(is);
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
    return vil2_image_data_sptr(0);
  }
}

vil2_image_data_sptr vil2_load_image_data(char const* filename)
{
  vil2_image_data_sptr im = vil2_load_image_data_raw(filename);
  if (!im) return im; // leave early if it hasn't loaded.

  bool top_first=true, bgr=false;
  im->get_property(vil2_property_top_row_first, &top_first);
  if (im->nplanes() == 3)
    im->get_property(vil2_property_component_order_is_BGR,&bgr);
#ifdef VIL2_TO_BE_FIXED
  if (!top_first)
    im = vil_flipud(im);
  if (bgr)
    im = vil_flip_components(im);
#endif// VIL2_TO_BE_FIXED
  return im;
}


//: Convenience function for loading an image into an image view.
vil2_image_view_base_sptr vil2_load(const char *file)
{
  vil2_image_data_sptr data = vil2_load_image_data(file);
  if (!data) return 0;
  return data -> get_view(0, data->ni(), 0, data->nj());
}
