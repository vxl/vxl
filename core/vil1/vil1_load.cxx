// This is core/vil1/vil1_load.cxx

#include <iostream>
#include "vil1_load.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vil1/vil1_open.h>
#include <vil1/vil1_file_format.h>
#include <vil1/vil1_stream.h>
#include <vil1/vil1_property.h>
#include <vil1/vil1_flipud.h>
#include <vil1/vil1_flip_components.h>

vil1_image vil1_load_raw(vil1_stream *is)
{
#ifdef DEBUG
  std::cout << __FILE__ " : trying" << std::flush;
#endif
  for (vil1_file_format** p = vil1_file_format::all(); *p; ++p) {
#ifdef DEBUG
    std::cout << " \'" << (*p)->tag() << "\'" << std::flush;
#endif
    is->seek(0);
    vil1_image i = (*p)->make_input_image(is);
    if (i && i.width()>=0 && i.height()>=0 && i.planes()>0 && i.components()>0 && i.bits_per_component()>0)
    {
#ifdef DEBUG
      std::cout << ": succeeded\n" << std::flush;
#endif
      return i;
    }
  }

  // failed.
  std::cerr << __FILE__ ": Tried";
  for (vil1_file_format** p = vil1_file_format::all(); *p; ++p)
    std::cerr << " \'" << (*p)->tag() << "\'" << std::flush;
  std::cerr << ": none succeeded\n";

  return nullptr;
}

vil1_image vil1_load_raw(char const* filename)
{
  vil1_stream *is = vil1_open(filename, "r");
  if (is) {
    vil1_image im = vil1_load_raw(is);
    // this will delete the stream object in case the loader has failed;
    // otherwise the specific image file format will have ref()ed "is"
    // in which case this does not harm.
    is->ref(); is->unref();
    return im;
  }
  else {
    std::cerr << __FILE__ ": Failed to load [" << filename << "]\n";
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
