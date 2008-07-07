// This is contrib/bvxm/pro/bvxm_detect_scale_process.h
#ifndef bvxm_detect_scale_process_h_
#define bvxm_detect_scale_process_h_
//:
// \file
//
//
//  CAUTION: Input image is assumed to have type vxl_byte
//
// \author Vishal Jain
// \date 04/15/2008
//
// \verbatim
// Modifications 
//
//
// \endverbatim

#include <vcl_string.h>
#include <vcl_algorithm.h>
#include <bprb/bprb_process.h>

#include <vil/vil_image_view.h>

class bvxm_detect_scale_process : public bprb_process
{
 public:

  bvxm_detect_scale_process();

  //: Copy Constructor (no local data)
  bvxm_detect_scale_process(const bvxm_detect_scale_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_detect_scale_process(){};

  //: Clone the process
  virtual bvxm_detect_scale_process* clone() const {return new bvxm_detect_scale_process(*this);}

  vcl_string name(){return "bvxmDetectScaleProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_detect_scale_process_h_
