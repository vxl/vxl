#ifndef RREL_EXPORT_H
#define RREL_EXPORT_H

#include <vxl_config.h> // get BUILD_SHARED_LIBS

#ifndef BUILD_SHARED_LIBS  // if not a shared build
# define vrel_EXPORT
#else  // this is a shared build
# ifdef vrel_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define vrel_EXPORT __declspec(dllexport)
#  else
#   define vrel_EXPORT
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define vrel_EXPORT __declspec(dllimport)
#  else
#   define vrel_EXPORT
#  endif
# endif
#endif

#endif
