// This is bocl_cl_gl.h
#ifndef bocl_cl_gl_h
#define bocl_cl_gl_h

//:
// \file
// \brief  Platform-independent includes for OpenCL/GL
// \author Isabel Restrepo mir@lems.brown.edu
// \date  24-Sep-2010.
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#include <CL/cl_gl.h>
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(UNIX) || defined(__linux__)
#define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
#include <OpenCL/cl_gl.h>
#include <OpenCL/cl_gl_ext.h>
#include <OpenGL/CGLDevice.h>
#define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
#endif


#endif
