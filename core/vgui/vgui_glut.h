#ifndef vgui_glut_h
#define vgui_glut_h

#if HAS_GLUT

// See vgui_glu.h
// There used to be logic here to deal issues related to GLUT's use of
// exit, but it appears to be unnecessary now. If the problem arises
// again, have a look the the older versions of this source, and in
// the comments at the top of glut.h
//  -- Amitha Perera

# include <vcl_compiler.h>
# include <vgui/vgui_gl.h>
# include <vgui/vgui_glu.h>
# include <GL/glut.h>

#else // HAS_GLUT
# error "Trying to use vgui_glut when HAS_GLUT is not defined."
// If you get here, then you are trying to use vgui_glut in your
// source without determining if your system has glut. If you are
// using CMake to build, then look at the FindGLUT module. Something like
//
//   INCLUDE(${MODULE_PATH}/NewCMake/FindGLUT.cmake)
//   IF( GLUT_FOUND )
//     INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
//     ADD_DEFINITIONS( -DHAS_GLUT)
//   ENDIF( GLUT_FOUND )

#endif // HAS_GLUT

#endif // vgui_glut_h
