// This is brl/bpro/core/vidl2_pro/vidl2_get_frame_istream_process.h
#ifndef vidl2_get_frame_istream_process_h_
#define vidl2_get_frame_istream_process_h_
//:
// \file
// \brief A process to advance to the given frame number in the vidl2_istream and return it 
// \author O. C. Ozcanli
// \date 12/18/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Loads an image list video istream
//  Inputs:  Index          Type                Purpose
//             0         vcl_string    Directory for the image list
//
class vidl2_get_frame_istream_process : public bprb_process
{
 public:

  vidl2_get_frame_istream_process();

  //: Copy Constructor (no local data)
  vidl2_get_frame_istream_process(const vidl2_get_frame_istream_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vidl2_get_frame_istream_process();

  //: Clone the process
  virtual vidl2_get_frame_istream_process* clone() const {return new vidl2_get_frame_istream_process(*this);}

  vcl_string name(){return "GetFrameVidl2IstreamProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vidl2_get_frame_istream_process_h_
