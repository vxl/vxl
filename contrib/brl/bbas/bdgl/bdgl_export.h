#ifndef BDGL_EXPORT_H
#define BDGL_EXPORT_H

#include <vxl_config.h> // get VXL_BUILD_SHARED_LIBS

#ifndef VXL_BUILD_SHARED_LIBS  // if not a shared build
# define bdgl_EXPORT_DATA
#else  // this is a shared build
# ifdef bdgl_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define bdgl_EXPORT_DATA __declspec(dllexport)
#  else
#   define bdgl_EXPORT_DATA
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define bdgl_EXPORT_DATA __declspec(dllimport)
#  else
#   define bdgl_EXPORT_DATA
#  endif
# endif
#endif

#endif
