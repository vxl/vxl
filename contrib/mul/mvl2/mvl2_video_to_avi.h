#ifndef mvl2_video_to_avi_h_
#define mvl2_video_to_avi_h_

//:
// \file
// \brief Includes the right file depending on the platform
// \author Franck Bettinger

#ifdef _MSC_VER
#include "mvl2/mvl2_video_to_avi_windows.h"
#else
#include "mvl2/mvl2_video_to_avi_linux.h"
#endif

#endif // mvl2_video_to_avi_h_
