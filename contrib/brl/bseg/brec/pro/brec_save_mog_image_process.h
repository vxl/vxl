// This is brl/bseg/brec/pro/brec_save_mog_image_process.h
#ifndef brec_save_mog_image_process_h_
#define brec_save_mog_image_process_h_
//:
// \file
// \brief A class for generating a synthetic brec_voxel_world from the input mixture of gaussian (mog) image.
//        basically a world of one slice 
//        creation of a mog image takes too long depending on the size of the voxel world used hence a process to save the output is required
//
// \author Ozge C. Ozcanli
// \date 12/15/2008
// \verbatim
//  Modifications
//
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_save_mog_image_process : public bprb_process
{
 public:

  brec_save_mog_image_process();

  //: Copy Constructor (no local data)
  brec_save_mog_image_process(const brec_save_mog_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_save_mog_image_process(){};

  //: Clone the process
  virtual brec_save_mog_image_process* clone() const {return new brec_save_mog_image_process(*this);}

  vcl_string name() { return "brecSaveMOGImageProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish(){return true; }

 private:

};

#endif // brec_save_mog_image_process_h_
