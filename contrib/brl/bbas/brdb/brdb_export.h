#ifndef BRDB_EXPORT_H
#define BRDB_EXPORT_H

#include <vxl_config.h> // get BUILD_SHARED_LIBS

#ifndef BUILD_SHARED_LIBS  // if not a shared build
# define brdb_EXPORT_DATA
#else  // this is a shared build
# ifdef brdb_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define brdb_EXPORT_DATA __declspec(dllexport)
#  else
#   define brdb_EXPORT_DATA
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define brdb_EXPORT_DATA __declspec(dllimport)
#  else
#   define brdb_EXPORT_DATA
#  endif
# endif
#endif
#endif
