#ifndef mvl2_video_to_avi_h_
#define mvl2_video_to_avi_h_

//:
// \file
// \brief Includes the right file depending on the platform
// \author Franck Bettinger

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#include "mvl2_video_to_avi_windows.h"
#else
#include "mvl2_video_to_avi_linux.h"
#endif

#endif // mvl2_video_to_avi_h_
