// This is core/vil1/vil1_load.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif

#include "vil1_load.h"

#include <vcl_iostream.h>

#include <vil1/vil1_open.h>
#include <vil1/vil1_file_format.h>
#include <vil1/vil1_stream.h>
#include <vil1/vil1_property.h>
#include <vil1/vil1_flipud.h>
#include <vil1/vil1_flip_components.h>

vil1_image vil1_load_raw(vil1_stream *is)
{
#ifdef DEBUG
  vcl_cout << __FILE__ " : trying" << vcl_flush;
#endif
  for (vil1_file_format** p = vil1_file_format::all(); *p; ++p) {
#ifdef DEBUG
    vcl_cout << " \'" << (*p)->tag() << "\'" << vcl_flush;
#endif
    is->seek(0);
    vil1_image i = (*p)->make_input_image(is);
    if (i && i.width()>=0 && i.height()>=0 && i.planes()>0 && i.components()>0 && i.bits_per_component()>0)
    {
#ifdef DEBUG
      vcl_cout << ": succeeded\n" << vcl_flush;
#endif
      return i;
    }
  }

  // failed.
  vcl_cerr << __FILE__ ": Tried";
  for (vil1_file_format** p = vil1_file_format::all(); *p; ++p)
    vcl_cerr << " \'" << (*p)->tag() << "\'" << vcl_flush;
  vcl_cerr << ": none succeeded\n";

  return 0;
}

vil1_image vil1_load_raw(char const* filename)
{
  vil1_stream *is = vil1_open(filename, "r");
  if (is) {
    vil1_image im = vil1_load_raw(is);
    delete is;
    return im;
  }
  else {
    vcl_cerr << __FILE__ ": Failed to load [" << filename << "]\n";
    return vil1_image();
  }
}

vil1_image vil1_load(char const* filename)
{
  vil1_image i = vil1_load_raw(filename);
  bool top_first=true, bgr=false;
  i.get_property(vil1_property_top_row_first, &top_first);
  if (i.components() == 3)
    i.get_property(vil1_property_component_order_is_BGR,&bgr);
  if (!top_first)
    i = vil1_flipud(i);
  if (bgr)
    i = vil1_flip_components(i);
  return i;
}
