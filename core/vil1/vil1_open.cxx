/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation "vil_open"
#endif
#include "vil_open.h"

#include <vcl_cstdio.h>   // sprintf()
#include <vcl_cstdlib.h>  // atoi()
#include <vcl_cstring.h>  // strncmp()

#include <vil/vil_stream_fstream.h>
#include <vil/vil_stream_core.h>
#include <vil/vil_stream_url.h>

vil_stream *vil_open(char const* what, char const* how)
{
  // check for null pointer or empty strings.
  if (!what || !*what)
    return 0;

  // try to open as file first.
  vil_stream *is = new vil_stream_fstream(what, how);
#if 0
  // unfortunately, the following doesn't work because (note typo)
  //    vil_open("/tmp/foo.jgp")
  // will create a new file, permissions allowing, instead of opening
  // the intended file /tmp/foo.jpg. suggest long-term solution is to
  // make a new vil_open*() set of functions which can open an image
  // for reading and/or writing depending on the caller's need and that
  // vil_load() just does a vil_open() for reading. i do not think people
  // expect "loading an image" to open the disk file for writing by
  // default. -- fsm
  vil_stream *is = new vil_stream_fstream(what, "r+");
#endif
  if (!is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }

  if (!is) {
    // hacked check for filenames beginning "gen:".
    int l = vcl_strlen(what);
    if (l > 4 && vcl_strncmp(what, "gen:", 4) == 0) {
      if (vcl_strcmp(how, "r") == 0) {
        // Make an in-core stream...
        vil_stream_core *cis = new vil_stream_core();
        cis->m_transfer((char*)what, 0, l+1, false/*write*/); /* const violation */
        is = cis;
      }
      else {
        vcl_cerr << __FILE__ ": cannot open gen:* for writing" << vcl_endl;
      }
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }

  if (!is) {
    // maybe it's a URL?
    int l = vcl_strlen(what);
    if (l > 4 && vcl_strncmp(what, "http://", 7) == 0) {
      if (vcl_strcmp(how, "r") == 0) {
        is = new vil_stream_url(what);
      }
      else
        vcl_cerr << __FILE__ ": cannot open URL for writing (yet)" << vcl_endl;
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }

  return is;
}
