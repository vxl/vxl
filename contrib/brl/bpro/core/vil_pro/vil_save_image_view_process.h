// This is brl/bpro/core/vil_pro/vil_save_image_process.h
#ifndef vil_save_image_view_process_h_
#define vil_save_image_view_process_h_

//:
// \file
// \brief A process for saving an image to disk
// \author Daniel Crispell
// \date 06/06/04
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>


class vil_save_image_view_process : public bprb_process
{
 public:
  
  vil_save_image_view_process();

  //: Copy Constructor (no local data)
  vil_save_image_view_process(const vil_save_image_view_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~vil_save_image_view_process();

  //: Clone the process
  virtual vil_save_image_view_process* clone() const {return new vil_save_image_view_process(*this);}

  vcl_string name(){return "SaveImageViewProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //vil_save_image_view_process_h_
