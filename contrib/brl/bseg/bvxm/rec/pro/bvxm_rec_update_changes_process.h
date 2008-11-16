// This is contrib/bvxm/rec/pro/bvxm_rec_update_changes_process.h
#ifndef bvxm_rec_update_changes_process_h_
#define bvxm_rec_update_changes_process_h_
//:
// \file
// \brief A class to update a change map iteratively based on background and foreground models
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli
// \date 04/10/2008
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

class bvxm_rec_update_changes_process : public bprb_process
{
 public:

  bvxm_rec_update_changes_process();

  //: Copy Constructor (no local data)
  bvxm_rec_update_changes_process(const bvxm_rec_update_changes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_rec_update_changes_process(){};

  //: Clone the process
  virtual bvxm_rec_update_changes_process* clone() const {return new bvxm_rec_update_changes_process(*this);}

  vcl_string name(){return "bvxmUpdateChangesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_rec_update_changes_process_h_
