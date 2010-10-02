// This is brl/bseg/boxm/ocl/view/boxm_ocl_include_glew.h
#ifndef boxm_ocl_include_glew_h
#define boxm_ocl_include_glew_h
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
#if defined(UNIX)
  #include <GL/glx.h>
#endif
#if defined(WIN32)
  #include <windows.h>
#endif
#if defined(__APPLE__) || defined(MACOSX)
  #include <GL/glxew.h>
  #include <OpenGL/OpenGL.h>
  #include <OpenGL/CGLDevice.h>
#endif

#endif
