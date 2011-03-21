// This is brl/bseg/boxm2/view/boxm2_include_glew.h
#ifndef boxm2_include_glew_h
#define boxm2_include_glew_h
//:
// \file
// \brief Platform independent includes for GLEW
// \author Isabel Restrepo mir@lems.brown.edu
// \date  24-Sep-2010.
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
