// This is brl/bpro/bipp/bipp_load_image_process.h
#ifndef bipp_load_image_process_h_
#define bipp_load_image_process_h_

//:
// \file
// \brief A process for loading an image into a frame
// \author Amir Tamrakar
// \date 06/06/04
//
// \verbatim
//  Modifications
//  02/04/08 Isabel Restrepo : Migrated to brl/bipp
// \endverbatim

#include <vcl_vector.h>
#include <vcl_string.h>
#include <bprb/bprb_process.h>

//: Loads an image into a video frame by creating a 
// bipp_image_storage class at that frame

class bipp_load_image_process : public bprb_process
{
 public:
  
  bipp_load_image_process();

  //: Copy Constructor (no local data)
  bipp_load_image_process(const bipp_load_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bipp_load_image_process();

  //: Clone the process
  virtual bipp_load_image_process* clone() const {return new bipp_load_image_process(*this);}

  vcl_string name(){return "LoadImageProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bipp_load_image_process_h_
