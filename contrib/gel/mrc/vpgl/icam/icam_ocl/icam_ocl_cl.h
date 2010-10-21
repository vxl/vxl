// This is gel/mrc/vpgl/icam/icam_ocl/icam_ocl_cl.h
#ifndef icam_ocl_cl_h
#define icam_ocl_cl_h
//:
// \file
// \brief  Platform-independent includes for OpenCL
// \author Isabel Restrepo mir@lems.brown.edu
// \date  December 4, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || \
    defined(UNIX) || defined(linux)
  #include <CL/cl.h>
  #include <CL/cl_gl.h>
  #define GL_SHARING_EXTENSION "cl_khr_gl_sharing"
#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)
  #include <OpenCL/cl.h>
  #include <OpenCL/cl_gl.h>
  #include <OpenCL/cl_gl_ext.h>
  #define GL_SHARING_EXTENSION "cl_APPLE_gl_sharing"
#else
  #include <opencl.h>
#endif


#endif // icam_ocl_cl_h
