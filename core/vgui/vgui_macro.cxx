// This is oxl/vgui/vgui_macro.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm@robots.ox.ac.uk

#include "vgui_macro.h"

#include <vcl_cstdlib.h>
#include <vcl_iostream.h>

#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

extern bool vgui_glerrors_are_bad;

void vgui_macro_error_reporter(char const *FILE, int LINE)
{
  static char const *last_file = 0;
  static int last_line = 0;
  static int numerrors = 0;

  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    if (last_file) {
      vcl_cerr << "Between " << last_file << ":" << last_line << '\n'
               << "    and " << FILE << ":" << LINE << " : \""
               << gluErrorString(err) << "\"\n";
    }
    else
      vcl_cerr << "In " << FILE << " before line " << LINE << " : "
               << gluErrorString(err) << '\n';

    if (++numerrors >= 20 && vgui_glerrors_are_bad) {
      vcl_cerr << "too many errors -- calling exit(1);\n";
      vcl_exit(1);
    }
  }

  // FIXME : this assumes that FILE points to static storage.
  last_file = FILE;
  last_line = LINE;
}
