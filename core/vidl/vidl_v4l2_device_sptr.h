// This is core/vidl/vidl_v4l2_device_sptr.h
#ifndef vidl_v4l2_device_sptr_h_
#define vidl_v4l2_device_sptr_h_
//:
// \file
// \brief A smart pointer to a device
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//   15 Apr 2008 Created (A. Garrido)
//\endverbatim

#include <vil/vil_smart_ptr.h>

class vidl_v4l2_device;

//: A smart pointer to a device
typedef vil_smart_ptr<vidl_v4l2_device> vidl_v4l2_device_sptr;

#endif // vidl_v4l2_device_sptr_h_
