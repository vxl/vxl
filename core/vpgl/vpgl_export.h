#ifndef VPGL_EXPORT_H
#define VPGL_EXPORT_H

#include <vxl_config.h> // get VXL_BUILD_SHARED_LIBS

#ifndef VXL_BUILD_SHARED_LIBS  // if not a shared build
# define VPGL_EXPORT
#else  // this is a shared build
# ifdef vpgl_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define VPGL_EXPORT __declspec(dllexport)
#  else
#   define VPGL_EXPORT
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define VPGL_EXPORT __declspec(dllimport)
#  else
#   define VPGL_EXPORT
#  endif
# endif
#endif

#endif
