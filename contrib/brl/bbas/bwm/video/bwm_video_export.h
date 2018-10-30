#ifndef BWM_VIDEO_EXPORT_H
#define BWM_VIDEO_EXPORT_H

#include <vxl_config.h> // get BUILD_SHARED_LIBS

#ifndef BUILD_SHARED_LIBS  // if not a shared build
# define bwm_video_EXPORT_DATA
#else  // this is a shared build
# ifdef bwm_video_EXPORTS  // if building this library
#  if defined(_WIN32) || defined(WIN32)
#   define bwm_video_EXPORT_DATA __declspec(dllexport)
#  else
#   define bwm_video_EXPORT_DATA
#  endif
# else // we are using this library and it is built shared
#  if defined(_WIN32) || defined(WIN32)
#   define bwm_video_EXPORT_DATA __declspec(dllimport)
#  else
#   define bwm_video_EXPORT_DATA
#  endif
# endif
#endif
#endif
