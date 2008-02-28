// This is brl/bpro/core/vpgl_pro/vpgl_load_rational_camera_process.h
#ifndef vpgl_load_rational_camera_process_h_
#define vpgl_load_rational_camera_process_h_
//:
// \file
// \brief A process for loading a vpgl_rational_camera from ascii file
// \author Daniel Crispell
// \date 02/06/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Loads a rational camera from a text file
class vpgl_load_rational_camera_process : public bprb_process
{
 public:

  vpgl_load_rational_camera_process();

  //: Copy Constructor (no local data)
  vpgl_load_rational_camera_process(const vpgl_load_rational_camera_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vpgl_load_rational_camera_process();

  //: Clone the process
  virtual vpgl_load_rational_camera_process* clone() const {return new vpgl_load_rational_camera_process(*this);}

  vcl_string name(){return "LoadRationalCameraProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vpgl_load_rational_camera_process_h_
