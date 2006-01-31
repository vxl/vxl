// This is brl/bbas/vidl2/vidl2_frame_sptr.h
#ifndef vidl2_frame_sptr_h_
#define vidl2_frame_sptr_h_
//:
// \file
// \brief A smart pointer to a video frame
//
// \author Matt Leotta
// \date 13 Jan 2006

#include <vil/vil_smart_ptr.h>

class vidl2_frame;

//: A smart pointer to a video frame
typedef vil_smart_ptr<vidl2_frame> vidl2_frame_sptr;

#endif // vidl2_frame_sptr_h_
