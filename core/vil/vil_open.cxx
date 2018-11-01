// This is core/vil/vil_open.cxx
//:
// \file
// \author fsm

#include <iostream>
#include <cstring>
#include <fstream>
#include "vil_open.h"


#include <vil/vil_stream_fstream.h>

#include <vil/vil_stream_core.h>
#include <vil/vil_stream_url.h>
// not used? #include <vcl_compiler.h>

vil_stream *vil_open(char const* what, char const* how)
{
  // check for null pointer or empty strings.
  if (!what || !*what)
    return nullptr;

  // try to open as file first.
#ifdef VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream64(what, how);
#else //VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream(what, how);
#endif //VIL_USE_FSTREAM64

#if 0
  // unfortunately, the following doesn't work because (note typo)
  //    vil_open<("/tmp/foo.jgp")
  // will create a new file, permissions allowing, instead of opening
  // the intended file /tmp/foo.jpg. suggest long-term solution is to
  // make a new vil_open<*() set of functions which can open an image
  // for reading and/or writing depending on the caller's need and that
  // vil_load() just does a vil_open<() for reading. i do not think people
  // expect "loading an image" to open the disk file for writing by
  // default. -- fsm
#ifdef VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream64(what, "r+");
#else //VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream(what, "r+");
#endif //VIL_USE_FSTREAM64
#endif
  if (!is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = nullptr;
  }

  if (!is) {
    // hacked check for filenames beginning "gen:".
    int l = (int)std::strlen(what);
    if (l > 4 && std::strncmp(what, "gen:", 4) == 0) {
      if (std::strcmp(how, "r") == 0) {
        // Make an in-core stream...
        auto *cis = new vil_stream_core();
        cis->write(what, l+1);
        is = cis;
      }
      else {
        std::cerr << __FILE__ ": cannot open gen:* for writing\n";
      }
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = nullptr;
  }

  if (!is) {
    // maybe it's a URL?
    int l = (int)std::strlen(what);
    if (l > 4 && std::strncmp(what, "http://", 7) == 0) {
      if (std::strcmp(how, "r") == 0) {
        is = new vil_stream_url(what);
      }
      else
        std::cerr << __FILE__ ": cannot open URL for writing (yet)\n";
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = nullptr;
  }

  return is;
}

#if defined(_WIN32) && VXL_USE_WIN_WCHAR_T
//  --------------------------------------------------------------------------------
//  Windows' wchar_t overloading version
//
//
vil_stream *vil_open(wchar_t const* what, char const* how)
{
  // check for null pointer or empty strings.
  if (!what || !*what)
    return 0;

  // try to open as file first.
#ifdef VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream64(what, how);
#else //VIL_USE_FSTREAM64
  vil_stream *is = new vil_stream_fstream(what, how);
#endif //VIL_USE_FSTREAM64

  if (!is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }

#if 0  // TODO: add wchar_t support in vil_stream_core
  if (!is) {
    // hacked check for filenames beginning "gen:".
    int l = wcslen(what);
    if (l > 4 && wcsncmp(what, L"gen:", 4) == 0) {
      if (std::strcmp(how, "r") == 0) {
        // Make an in-core stream...
        vil_stream_core *cis = new vil_stream_core();
        cis->write(what, l+1);
        is = cis;
      }
      else {
        std::cerr << __FILE__ ": cannot open gen:* for writing\n";
      }
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }
#endif

#if 0  // TODO: add wchar_t support in vil_stream_url
  if (!is) {
    // maybe it's a URL?
    int l = std::strlen(what);
    if (l > 4 && std::strncmp(what, "http://", 7) == 0) {
      if (std::strcmp(how, "r") == 0) {
        is = new vil_stream_url(what);
      }
      else
        std::cerr << __FILE__ ": cannot open URL for writing (yet)\n";
    }
  }
  if (is && !is->ok()) {
    // this will delete the stream object.
    is->ref();
    is->unref();
    is = 0;
  }
#endif

  return is;
}

#endif //defined(_WIN32) && VXL_USE_WIN_WCHAR_T
