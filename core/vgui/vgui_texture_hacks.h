#ifndef vgui_texture_hacks_h_
#define vgui_texture_hacks_h_
#ifdef __GNUC__
#pragma interface
#endif
// .NAME vgui_texture_hacks
// .INCLUDE vgui/vgui_texture_hacks.h
// .FILE vgui_texture_hacks.cxx
//
// @author fsm@robots.ox.ac.uk

#include <vgui/vgui_gl.h>

// ask fsm@robots.ox.ac.uk about these hacks.
#if defined(__sun__) && defined(__gl_h_)
# define fsm_fake_gl_texture_calls
GLboolean urgh_glAreTexturesResident(GLsizei, GLuint const *, GLboolean *);
void      urgh_glBindTexture(GLenum, GLuint);
void      urgh_glDeleteTextures(GLsizei, GLuint const *);
void      urgh_glGenTextures(GLsizei, GLuint *);
GLboolean urgh_glIsTexture(GLuint );
void      urgh_glPrioritizeTextures(GLsizei, GLuint const *, GLclampf const *);

# define glAreTexturesResident urgh_glAreTexturesResident
# define glBindTexture         urgh_glBindTexture
# define glDeleteTextures      urgh_glDeleteTextures
# define glGenTextures         urgh_glGenTextures
# define glIsTexture           urgh_glIsTexture
# define glPrioritizeTextures  urgh_glPrioritizeTextures
#endif

#endif // vgui_texture_hacks_h_
