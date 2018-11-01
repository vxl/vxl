// This is core/vgui/vgui_texture_hacks.h
#ifndef vgui_texture_hacks_h_
#define vgui_texture_hacks_h_
//:
// \file
// \author fsm
// \brief  Ask fsm about these hacks.

#include <vgui/vgui_gl.h>

// ask fsm about these hacks.
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
