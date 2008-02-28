// This is brl/bpro/core/vil_pro/vil_load_image_view_process.h
#ifndef vil_load_image_view_process_h_
#define vil_load_image_view_process_h_
//:
// \file
// \brief A process for loading an image into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
//   02/04/08 Isabel Restrepo : Migrated to brl/bipp
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Loads an image into a video frame by creating a
// bipp_image_storage class at that frame
//
class vil_load_image_view_process : public bprb_process
{
 public:

  vil_load_image_view_process();

  //: Copy Constructor (no local data)
  vil_load_image_view_process(const vil_load_image_view_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_load_image_view_process();

  //: Clone the process
  virtual vil_load_image_view_process* clone() const {return new vil_load_image_view_process(*this);}

  vcl_string name(){return "LoadImageViewProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif //vil_load_image_view_process_h_
