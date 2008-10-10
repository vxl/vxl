// This is brl/bpro/bvgl_pro/bvgl_generate_mask_process.h
#ifndef bvgl_generate_mask_process_h_
#define bvgl_generate_mask_process_h_
//:
// \file
// \brief A process for generating a bool image as a mask from the binary ground truth changes class
// \author Ozge C. Ozcanli 
// \date October 9, 2008
//
// \verbatim
//  Modifications
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvgl_generate_mask_process : public bprb_process
{
 public:

  bvgl_generate_mask_process();

  //: Copy Constructor (no local data)
  bvgl_generate_mask_process(const bvgl_generate_mask_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bvgl_generate_mask_process();

  //: Clone the process
  virtual bvgl_generate_mask_process* clone() const {return new bvgl_generate_mask_process(*this);}

  vcl_string name(){return "bvglGenerateMaskProcess";}

  bool init(){return true;}
  bool execute();
  bool finish(){return true;}
};

#endif //bvgl_generate_mask_process_h_
