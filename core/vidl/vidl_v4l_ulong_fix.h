// This is core/vidl/vidl_v4l_ulong_fix.h
#ifndef vidl_v4l2_ulong_fix_h_
#define vidl_v4l2_ulong_fix_h_
//:
// \file
// \brief Fix system bug where type ulong is not defined in linux/videodev.h on some Linux/GCC combinations.
//
// \author Fred Wheeler
//
// I have found that on a Red Hat Linux 2.6.9 64 bit system with gcc
// 4.3.3, in order to build vidl, you need to #define __USE_MISC before
// #including sys/types.h and you need to make sure sys/types.h is
// #included in order to get ulong typedefed.  ulong is used by
// /usr/include/linux/videodev.h which is included by several VIDL CXX
// files.  I see some indications on the web that this ulong problem
// is a bug with certain Linux/GCC combinations.
//
// This header file implements this fix.  When it is needed it should
// be the first include.
//
// -Fred Wheeler

#if 1 // #ifdef VXL_VIDL_USE_ULONG_FIX
// We could use this condition to limit use of this fix.
// For now it is enabled for all - change if this fix causes any problems.

#define __USE_MISC
// including sys/types.h with this define will get ulong typedefed
#include <sys/types.h>

#endif // VXL_VIDL_USE_ULONG_FIX

#endif // vidl_v4l2_ulong_fix_h_
