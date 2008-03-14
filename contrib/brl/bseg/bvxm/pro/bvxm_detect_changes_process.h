// This is contrib/brl/bseg/bvxm/pro/bvxm_detect_changes_process.h
#ifndef bvxm_detect_changes_process_h_
#define bvxm_detect_changes_process_h_

//:
// \file
// \brief // A class for detecting changes using a voxel world . 
//           
// \author Daniel Crispell
// \date 02/10/08
// \verbatim
//
// \Modifications 

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_detect_changes_process : public bprb_process
{
 public:
  
   bvxm_detect_changes_process();

  //: Copy Constructor (no local data)
  bvxm_detect_changes_process(const bvxm_detect_changes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_detect_changes_process(){};

  //: Clone the process
  virtual bvxm_detect_changes_process* clone() const {return new bvxm_detect_changes_process(*this);}

  vcl_string name(){return "bvxmDetectChangesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};



#endif // bvma_update_process_h_

