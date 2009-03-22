#ifndef vidl1_avicodec_h_
#define vidl1_avicodec_h_

//:
// \file
// \brief Includes the right avi codec header depending on the platform
// \author Matt Leotta

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#include "vidl1_win_avicodec.h"
#else
#include "vidl1_avifile_avicodec.h"
#endif

#endif // vidl1_avicodec_h_
