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

// Workaround for a "bug" in GL/glut.h, where the signature of glutAddMenuEntry
// and some others is (char*,int) instead of (const char*,long).
inline void glutAddMenuEntry(const char* label, int value) { glutAddMenuEntry(const_cast<char*>(label),value); }
inline void glutAddMenuEntry(const char* label, long value) { glutAddMenuEntry(const_cast<char*>(label),int(value)); }
inline int glutCreateWindow(const char* title) { return glutCreateWindow(const_cast<char*>(title)); }
inline void glutSetWindowTitle(const char* title) { glutSetWindowTitle(const_cast<char*>(title)); }
inline void glutSetIconTitle(const char* title) { glutSetIconTitle(const_cast<char*>(title)); }
inline void glutAddSubMenu(const char* label, int sub) { glutAddSubMenu(const_cast<char*>(label),sub); }
inline void glutAddSubMenu(const char* label, long sub) { glutAddSubMenu(const_cast<char*>(label),int(sub)); }
inline void glutChangeToMenuEntry(int item, const char* label, int value) {
  glutChangeToMenuEntry(item,const_cast<char*>(label),value); }
inline void glutChangeToMenuEntry(long item, const char* label, long value) {
  glutChangeToMenuEntry(int(item),const_cast<char*>(label),int(value)); }
inline void glutChangeToSubMenu(int item, const char* label, int sub) {
  glutChangeToSubMenu(item,const_cast<char*>(label),sub); }
inline void glutChangeToSubMenu(long item, const char* label, long sub) {
  glutChangeToSubMenu(int(item),const_cast<char*>(label),int(sub)); }
inline int glutExtensionSupported(const char* name) { return glutExtensionSupported(const_cast<char*>(name)); }

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
