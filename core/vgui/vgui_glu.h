#ifndef vgui_glu_h
#define vgui_glu_h

// Since GL/glu.h will #include GL/gl.h, which is broken, we cannot
// #include GL/glu.h directly either -- fsm.

#include <vgui/vgui_gl.h>
#ifdef __APPLE__
# include <glu.h>
#else
# include <GL/glu.h>
#endif

#endif // vgui_glu_h
