// This is bocl_cl.h
#ifndef bocl_cl_h
#define bocl_cl_h

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

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)

#include <CL/cl.h>

#elif defined(macintosh) || defined(__APPLE__) || defined(__APPLE_CC__)

#include <cl.h>

#endif

#endif
