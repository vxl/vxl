#ifndef vgui_gl_h
#define vgui_gl_h
//:
// \file

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgui/vgui_config.h>

#if defined(_WIN32) && !defined(__CYGWIN__)
// fix windows gl.h :
# include <windows.h>
# ifdef _MSC_VER // exclude when using MinGW
#  pragma warning (disable:4244) // disable bogus conversion warnings
# endif
# include <GL/gl.h>
# undef min
# undef max
// Fix wxWidgets incompatibility with windows.h. Some macros defined by
// windows.h have the same name as wxWidgets classes.
# ifdef __WXMSW__
#  include <wx/msw/winundef.h>
# endif
#elif defined(__APPLE__)
#include <OpenGL/gl.h>
#else
// no need to fix :
# include <GL/gl.h>
#endif

// If the value is defined from outside, don't try to guess
#ifndef VGUI_MESA
#  define VGUI_MESA 0
#  if (defined(MESA) || defined(GL_MESA_window_pos) || defined(GL_MESA_resize_buffers))
    // xmesa.h is not distributed with Cygwin
#    ifndef __CYGWIN__
#    ifndef NO_MESA
#      undef VGUI_MESA
#      define VGUI_MESA 1
#    endif
#    endif
#  endif
#endif

// capes - Some defines for OpenGL < v1.2 implementations
// The numbers are those in the GL 1.2 spec.
// These are only used in mapping between GL formats and
// accelerated renderers, so no harm done...
#ifndef GL_ABGR_EXT
#  define GL_ABGR_EXT 0x8000
#endif
#ifndef GL_BGR
#  define GL_BGR 0x80E0
#endif
#ifndef GL_BGRA
#  define GL_BGRA 0x80E1
#endif
#ifndef GL_UNSIGNED_SHORT_5_6_5
#  define GL_UNSIGNED_SHORT_5_6_5 0x8363
#endif

// awf: more (similar) defines for win32...
#ifndef GL_UNSIGNED_SHORT_5_5_5_1
#  define GL_UNSIGNED_SHORT_5_5_5_1  0x8034
#endif
#ifndef GL_UNSIGNED_SHORT_1_5_5_5_REV
#  define GL_UNSIGNED_SHORT_1_5_5_5_REV  0x8066
#endif

#endif // vgui_gl_h
