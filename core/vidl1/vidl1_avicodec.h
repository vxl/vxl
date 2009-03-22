#ifndef vidl_avicodec_h_
#define vidl_avicodec_h_

//:
// \file
// \brief Includes the right avi codec header depending on the platform
// \author Matt Leotta

#if defined(_MSC_VER) || defined(__BORLANDC__) || defined(__MINGW32__)
#include "vidl_win_avicodec.h"
#else
#include "vidl_avifile_avicodec.h"
#endif

#endif // vidl_avicodec_h_
