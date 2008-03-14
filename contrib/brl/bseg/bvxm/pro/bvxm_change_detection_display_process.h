// This is /pro/bvxm_change_detection_display_process.h
#ifndef bvxm_change_detection_display_process_h_
#define bvxm_change_detection_display_process_h_

//:
// \file
// \brief // process to threshold the changes for foreground detection.
//           
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_change_detection_display_process : public bprb_process
{
 public:
  
   bvxm_change_detection_display_process();

  //: Copy Constructor (no local data)
  bvxm_change_detection_display_process(const bvxm_change_detection_display_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_change_detection_display_process(){};

  //: Clone the process
  virtual bvxm_change_detection_display_process* clone() const {return new bvxm_change_detection_display_process(*this);}

  vcl_string name(){return "bvxmChangeDetectionDisplayProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvxm_change_detection_display_process

