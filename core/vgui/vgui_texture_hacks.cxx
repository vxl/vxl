/*
  fsm@robots.ox.ac.uk
*/
#ifdef __GNUC__
#pragma implementation
#endif
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
    if (debug) printf("dlsym(%s) => 0x%08X\n", #name, unsigned(f)); \
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






#if 0
#if defined(__sun__) && defined(__gl_h_)
// fsm: I can see these symbols in /usr/openwin/lib/libGL.so, but
// they're not in the header /usr/openwin/include/GL/gl.h
//
// This should eventually be autoconfed and inserted into vgui_gl.h
extern "C" {
  GLboolean glAreTexturesResident(GLsizei, GLuint const *, GLboolean *);
  void      glBindTexture(GLenum, GLuint);
  void      glDeleteTextures(GLsizei, GLuint const *);
  void      glGenTextures(GLsizei, GLuint *);
  GLboolean glIsTexture(GLuint );
  void      glPrioritizeTextures(GLsizei, GLuint const *, GLclampf const *);
}
#endif
#endif
