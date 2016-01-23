#ifndef VGL_EXPORT_H
#define VGL_EXPORT_H

#include <vxl_config.h> // get VXL_BUILD_SHARED_LIBS

#ifndef VXL_BUILD_SHARED_LIBS  // if not a shared build
# define VGL_EXPORT
#else  // this is a shared build
# ifdef vgl_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define VGL_EXPORT __declspec(dllexport)
#  else
#   define VGL_EXPORT
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define VGL_EXPORT __declspec(dllimport)
#  else
#   define VGL_EXPORT
#  endif
# endif
#endif

#endif
