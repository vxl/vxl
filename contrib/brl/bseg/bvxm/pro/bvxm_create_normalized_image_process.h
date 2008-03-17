// This is contrib/brl/bseg/bvxm/pro/bvxm_create_normalized_image_process.h
#ifndef bvxm_create_normalized_image_process_h_
#define bvxm_create_normalized_image_process_h_

//:
// \file
// \brief // A class to create a contrast normalized image using the input gain and offset values
//           
// \author Ozge Can Ozcanli
// \date 02/17/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>
#include <vcl_cmath.h>

class bvxm_create_normalized_image_process : public bprb_process
{
 public:
  
  bvxm_create_normalized_image_process();

  //: Copy Constructor (no local data)
  bvxm_create_normalized_image_process(const bvxm_create_normalized_image_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_create_normalized_image_process(){};

  //: Clone the process
  virtual bvxm_create_normalized_image_process* clone() const {return new bvxm_create_normalized_image_process(*this);}

  vcl_string name(){return "bvxmCreateNormalizedImageProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif // bvxm_create_normalized_image_process_h_

