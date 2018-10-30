#ifndef MBL_EXPORT_H
#define MBL_EXPORT_H

#include <vxl_config.h> // get BUILD_SHARED_LIBS

#ifndef BUILD_SHARED_LIBS  // if not a shared build
# define MBL_EXPORT
#else  // this is a shared build
# ifdef mbl_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define MBL_EXPORT __declspec(dllexport)
#  else
#   define MBL_EXPORT
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define MBL_EXPORT __declspec(dllimport)
#  else
#   define MBL_EXPORT
#  endif
# endif
#endif

#endif
