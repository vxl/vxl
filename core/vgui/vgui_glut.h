#ifndef vgui_glut_h
#define vgui_glut_h

#if HAS_GLUT

// See vgui_glu.h
// There used to be logic here to deal issues related to GLUT's use of
// exit, but it appears to be unnecessary now. If the problem arises
// again, have a look the the older versions of this source, and in
// the comments at the top of glut.h
//  - Amitha Perera

# include <vcl_compiler.h>
# include <vgui/vgui_gl.h>
# include <vgui/vgui_glu.h>
# include <GL/glut.h>

// Workaround for a "bug" in GL/glut.h on Leuven's alpha, where the
// prototypes of some functions is declared as char* instead of const
// char*. Note that we cannot (should not) overload these functions
// since they are C functions. Instead we define the local functions
// with the correct prototypes (lifted from the GLUT 3.7 sources), and
// call the system ones to do the actual work. We then use the
// pre-processor to use our functions instead of the incorrect system
// declarations.
//
// We expect and hope that the broken implementations don't attempt to
// modify the constant strings.
//
// The "fixing functions" have C++ linkage, but that's okay, since
// vgui is a C++ library anyway.

// The #ifdef test below is based on one
// sample. GLUT_XLIB_IMPLEMENTATION==5 is circa 1995. Most modern GLUT
// libraries are probably 11 or greater.
#if GLUT_XLIB_IMPLEMENTATION <= 5 // fix glut function declarations

inline int vgui_glutCreateWindow( const char* title )
{
  return glutCreateWindow( const_cast<char*>(title) );
}
#undef glutCreateWindow
#define glutCreateWindow   vgui_glutCreateWindow

inline void vgui_glutSetWindowTitle( const char* title )
{
  glutSetWindowTitle( const_cast<char*>(title) );
}
#undef glutSetWindowTitle
#define glutSetWindowTitle vgui_glutSetWindowTitle

inline void vgui_glutSetIconTitle( const char* title )
{
  glutSetIconTitle( const_cast<char*>(title) );
}
#undef glutSetIconTitle
#define glutSetIconTitle   vgui_glutSetIconTitle

inline void vgui_glutAddMenuEntry( const char* label, int value )
{
  glutAddMenuEntry( const_cast<char*>(label), value );
}
#undef glutAddMenuEntry
#define glutAddMenuEntry   vgui_glutAddMenuEntry

inline void vgui_glutAddSubMenu( const char* label, int sub )
{
  glutAddSubMenu( const_cast<char*>(label), sub );
}
#undef glutAddSubMenu
#define glutAddSubMenu     vgui_glutAddSubMenu

inline void vgui_glutChangeToMenuEntry( int item, const char* label, int value )
{
  glutChangeToMenuEntry( item, const_cast<char*>(label), value );
}
#undef glutChangeToMenuEntry
#define glutChangeToMenuEntry vgui_glutChangeToMenuEntry

inline void vgui_glutChangeToSubMenu( int item, const char* label, int sub )
{
  glutChangeToSubMenu( item, const_cast<char*>(label), sub );
}
#define glutChangeToSubMenu vgui_glutChangeToSubMenu

#if (GLUT_API_VERSION >= 2)
inline int vgui_glutExtensionSupported( const char* name )
{
  return glutExtensionSupported( const_cast<char*>(name) );
}
#undef glutExtensionSupported
#define glutExtensionSupported vgui_glutExtensionSupported
#endif

#endif // fix glut function declarations

#else // HAS_GLUT
# error "Trying to use vgui_glut when HAS_GLUT is not defined."
// If you get here, then you are trying to use vgui_glut in your
// source without determining if your system has glut. If you are
// using CMake to build, then look at the FindGLUT module. Something like
//
//   INCLUDE(${CMAKE_ROOT}/Modules/FindGLUT.cmake)
//   IF( GLUT_FOUND )
//     INCLUDE_DIRECTORIES(${GLUT_INCLUDE_DIR})
//     ADD_DEFINITIONS( -DHAS_GLUT)
//   ENDIF( GLUT_FOUND )

#endif // HAS_GLUT

#endif // vgui_glut_h
