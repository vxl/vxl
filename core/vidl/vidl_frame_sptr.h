// This is core/vidl/vidl_frame_sptr.h
#ifndef vidl_frame_sptr_h_
#define vidl_frame_sptr_h_
//:
// \file
// \brief A smart pointer to a video frame
//
// \author Matt Leotta
// \date 13 Jan 2006

#include <vil/vil_smart_ptr.h>
#include <vidl/vidl_frame.h>

//: A smart pointer to a video frame
typedef vil_smart_ptr<vidl_frame> vidl_frame_sptr;

#endif // vidl_frame_sptr_h_
