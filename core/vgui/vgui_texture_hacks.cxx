// This is oxl/vgui/vgui_texture_hacks.cxx
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma implementation
#endif
//:
// \file
// \author fsm
// \brief  See vgui_texture_hacks.h for a description of this file.

#include "vgui_texture_hacks.h"

#ifdef fsm_fake_gl_texture_calls

#include <dlfcn.h>
#include <vcl_cstdio.h>
#include <vcl_cassert.h>

static bool debug = false;
#define return_GLboolean(x) return x
#define return_void(x) { x; return; }

// ret: return value type.
// name: name of function to wrap.
// proto: prototype for function.
// args: argument list for function.
#define macro(ret, name, proto, args) \
ret urgh_##name proto \
{ \
  static void *f = 0; \
  if (!f) { \
    f = dlsym(RTLD_DEFAULT, #name); \
    if (debug) vcl_printf("dlsym(%s) => 0x%08X\n", #name, unsigned(f)); \
    if (! f) { \
      assert(false); \
      return_##ret( ret() ); \
    } \
  } \
  return_##ret( ((ret (*) proto) f) args); \
}

macro(GLboolean, glAreTexturesResident, (GLsizei a, GLuint const *b, GLboolean *c), (a, b, c));
macro(void,      glBindTexture, (GLenum a, GLuint b), (a, b));
macro(void,      glDeleteTextures, (GLsizei a, GLuint const *b), (a, b));
macro(void,      glGenTextures, (GLsizei a, GLuint *b), (a, b));
macro(GLboolean, glIsTexture, (GLuint a), (a));
macro(void,      glPrioritizeTextures, (GLsizei a, GLuint const *b, GLclampf const *c), (a, b, c));

#undef macro

#endif
