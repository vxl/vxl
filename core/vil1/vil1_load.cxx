// This is ./vxl/vil/vil_load.cxx
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_load.h"

#include <vcl_iostream.h>

#include <vil/vil_open.h>
#include <vil/vil_file_format.h>
#include <vil/vil_stream.h>
#include <vil/vil_property.h> // for vil_property_top_row_first
#include <vil/vil_flipud.h>
#include <vil/vil_flip_components.h>

vil_image vil_load_raw(vil_stream *is)
{
  for (vil_file_format** p = vil_file_format::all(); *p; ++p) {
#if 0 // debugging
    vcl_cerr << __FILE__ " : trying \'" << (*p)->tag() << "\'" << vcl_endl;
#endif
    is->seek(0);
    vil_image i = (*p)->make_input_image(is);
    if (i)
      return i;
  }

  // failed.
  vcl_cerr << __FILE__ ": Tried";
  for (vil_file_format** p = vil_file_format::all(); *p; ++p)
    // 'flush' in case of segfault next time through loop. Else, we
    // will not see those printed tags still in the stream buffer.
    vcl_cerr << " \'" << (*p)->tag() << "\'" << vcl_flush;
  vcl_cerr << vcl_endl;

  return 0;
}

vil_image vil_load_raw(char const* filename)
{
  vil_stream *is = vil_open(filename, "r");
  if (is)
    return vil_load_raw(is);
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]" << vcl_endl;
    return vil_image();
  }
}

vil_image vil_load(char const* filename)
{
  vil_image i = vil_load_raw(filename);
  bool top_first=true, bgr=false;
  i.get_property(vil_property_top_row_first, &top_first);
  if (i.components() == 3)
    i.get_property(vil_property_component_order_is_BGR,&bgr);
  if (!top_first)
    i = vil_flipud(i);
  if (bgr)
    i = vil_flip_components(i);
  return i;
}
