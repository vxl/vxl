#ifndef vgui_glut_h
#define vgui_glut_h

// See vgui_glu.h

#include <vcl_compiler.h>
#include <vgui/vgui_gl.h>
#include <vgui/vgui_glu.h>

#ifdef xxxVCL_WIN32
# define VGUI_GLUT_PRESENT 0
#else
# define VGUI_GLUT_PRESENT 1
# if 0 // fsm
// It seems that this hack is no longer necessary. For Glut 3.7(.2) under
// Windows, the comments indicate that they do a work around for the exit
// fiasco. The hack seems to be unnecessary for new Linux boxen, and is
// unnessary for FreeBSD. I suspect it is unnessary on other, relatively
// recent Unixen - perera@cs.rpi.edu

// It looks like the original #ifdef below had its condition reversed.
// Also, it looks like Windows needs some serious hacking here, which
// is the reason for the #ifdef being here.  For more info, check the
// explanation in a comment around the top of glut.h
//  - Atanas Georgiev <atanas@cs.columbia.edu>
# ifdef VCL_WIN32
   extern "C" void vgui_glut_exit(int);
#  define exit vgui_glut_exit
# endif
#  include <GL/glut.h>
#  undef exit
# else
#  include <GL/glut.h>
# endif
#endif

#endif // vgui_glut_h
