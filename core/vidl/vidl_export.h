#ifndef VIDL_EXPORT_H
#define VIDL_EXPORT_H

#include <vxl_config.h> // get VXL_BUILD_SHARED_LIBS

#ifndef VXL_BUILD_SHARED_LIBS  // if not a shared build
#  define VIDL_EXPORT
#else  // this is a shared build
#  ifdef vidl_EXPORTS // if building this library
#    if defined(_WIN32) || defined(WIN32)
#      define VIDL_EXPORT __declspec(dllexport)
#    else
#      define VIDL_EXPORT
#    endif
#  else // we are using this library and it is built shared
#    if defined(_WIN32) || defined(WIN32)
#      define VIDL_EXPORT __declspec(dllimport)
#    else
#      define VIDL_EXPORT
#    endif
#  endif
#endif

#endif
