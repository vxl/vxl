// This is ./vxl/vil/vil_load.cxx
#ifdef __GNUC__
#pragma implementation
#endif

#include "vil_load.h"

#include <vcl_cstdio.h>   // sprintf()
#include <vcl_cstdlib.h>  // atoi()
#include <vcl_cstring.h>
#include <vcl_iostream.h>

#include <vil/vil_file_format.h>
#include <vil/vil_stream_fstream.h>
#include <vil/vil_stream_core.h>
#include <vil/vil_stream_url.h>
#include <vil/vil_image.h>
#include <vil/vil_property.h>
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
  vcl_cerr << "vil_load: Tried";
  for (vil_file_format** p = vil_file_format::all(); *p; ++p)
    // 'flush' in case of segfault next time through loop. Else, we
    // will not see those printed tags still in the stream buffer.
    vcl_cerr << " \'" << (*p)->tag() << "\'" << vcl_flush;
  vcl_cerr << vcl_endl;

  return 0;
}

vil_image vil_load_raw(char const* filename)
{
  // check for null pointer or empty strings.
  if (!filename || !*filename)
    return 0;

#if 1
  vil_stream *is = new vil_stream_fstream(filename, "r");
  is->ref();
#else
  // unfortunately, the following doesn't work because (note typo)
  //    vil_open("/tmp/foo.jgp")
  // will create a new file, permissions allowing, instead of opening
  // the intended file /tmp/foo.jpg. suggest long-term solution is to
  // make a new vil_open*() set of functions which can open an image
  // for reading and/or writing depending on the caller's need and that
  // vil_load() just does a vil_open() for reading. i do not think people
  // expect "loading an image" to open the disk file for writing by
  // default. -- fsm
  vil_stream *is = new vil_stream_fstream(filename, "r+");
  is->ref();
  // the file might not be writable.
  if (! is->ok()) {
    is->unref();
    is = new vil_stream_fstream(filename, "r");
    is->ref();
  }
#endif
  // current block scope is owner of stream.

  if (!is->ok()) {
    int l = vcl_strlen(filename);

    // hacked check for filenames beginning "gen:".
    if (l > 4 && vcl_strncmp(filename, "gen:", 4) == 0) {
      is->unref();
      // Make an in-core stream...
      vil_stream_core *cis = new vil_stream_core();
      cis->ref();
      cis->write(filename, l+1);
      is = cis;
    }

    // maybe it's a URL?
    else if (l > 7 && vcl_strncmp(filename, "http://", 7) == 0) {
      is->unref();
      is = new vil_stream_url(filename);
      is->ref();
    }
  }

  if (!is->ok()) {
    is->unref();
    // end of block scope coming up.
    return 0;
  }

  vil_image img = vil_load_raw(is);
  // stream has up to two owners now.

  is->unref();
  is = 0;
  // block scope no longer owns stream.

  if (! img)
    vcl_cerr << "vil_load: Failed to load [" << filename << "]" << vcl_endl;

  return img;
}

vil_image vil_load(char const* filename)
{
  vil_image i = vil_load_raw(filename);
  bool top_first, bgr;
  if (i.get_property(vil_property_top_row_first, &top_first) && !top_first)
    i = vil_flipud(i);
  if (i.components() == 3 && i.get_property(vil_property_component_order_is_BGR,&bgr) && bgr)
    i = vil_flip_components(i);
  return i;
}
