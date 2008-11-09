// This is brl/bbas/vidl2/vidl2_v4l2_devices.h
#ifndef vidl2_v4l2_devices_h_
#define vidl2_v4l2_devices_h_
//:
// \file
// \brief A class for a singleton with video devices
//
// \author Antonio Garrido
// \verbatim
//  Modifications
//     15 Apr 2009 Created (A. Garrido)
//\endverbatim

#include <vcl_vector.h>
#include "vidl2_v4l2_device.h"
#include "vidl2_v4l2_device_sptr.h"


//: A class which stores all devices in a object
// This object is a singleton initialized automatically by detecting all devices in the system,
// The devices are detected iterating over all files in the device directory.
//
// This singleton can be accessed as vidl2_v4l2_devices::all()
// Note that you can work with devices without using this singleton, because you can
// create a vidl2_v4l2_device directly from the file name. However, it is better to
// look for the device in this singleton using directly de card name.
// \see vidl2_v4l2_device
class vidl2_v4l2_devices
{
  vcl_vector<vidl2_v4l2_device_sptr> vecdev;
  vidl2_v4l2_devices();
  void load_devices(const char *name);
 public:
  //: Instance
  static vidl2_v4l2_devices& all();

  //: Return number of detected devices in the system
  int size() const { return vecdev.size(); }
  //: Return device i (0..size()-1)
  vidl2_v4l2_device& device(unsigned int i) {return *vecdev[i];}
};

#endif // vidl2_v4l2_devices_h_
