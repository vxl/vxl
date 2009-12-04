// This is bcl_cl.h
#ifndef bcl_cl_h
#define bcl_cl_h

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
