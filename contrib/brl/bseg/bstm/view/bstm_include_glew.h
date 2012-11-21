// This is brl/bseg/bstm/view/bstm_include_glew.h
#ifndef bstm_include_glew_h
#define bstm_include_glew_h
//:
// \file
// \brief Platform independent includes for GLEW
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <GL/glew.h>
#if defined(WIN32)
  #include <windows.h>
#elif defined(__APPLE__) || defined(MACOSX)
  #include <GL/glxew.h>
  #include <OpenGL/OpenGL.h>
  #include <OpenGL/CGLDevice.h>
#else //defined(UNIX)
  #include <GL/glx.h>
#endif


#endif
