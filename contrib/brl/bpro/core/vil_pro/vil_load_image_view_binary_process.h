// This is brl/bpro/core/vil_pro/vil_load_image_view_binary_process.h
#ifndef vil_load_image_view_binary_process_h_
#define vil_load_image_view_binary_process_h_
//:
// \file
// \brief A process for loading an image from a binary file into a frame
// \author Ozge C. Ozcanli
// \date 10/09/2008
//
// \verbatim
//  Modifications
//   
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Loads an image into a video frame by creating a bipp_image_storage class at that frame
//
class vil_load_image_view_binary_process : public bprb_process
{
 public:

  vil_load_image_view_binary_process();

  //: Copy Constructor (no local data)
  vil_load_image_view_binary_process(const vil_load_image_view_binary_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_load_image_view_binary_process();

  //: Clone the process
  virtual vil_load_image_view_binary_process* clone() const {return new vil_load_image_view_binary_process(*this);}

  vcl_string name(){return "LoadImageViewBinaryProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vil_load_image_view_binary_process_h_
