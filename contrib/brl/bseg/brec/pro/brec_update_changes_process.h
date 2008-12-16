// This is brl/bseg/brec/pro/brec_update_changes_process.h
#ifndef brec_update_changes_process_h_
#define brec_update_changes_process_h_
//:
// \file
// \brief A class to update a change map iteratively based on background and foreground models
//
//  CAUTION: Input image is assumed to be a probability map, i.e. have type float with values in [0,1]
//
// \author Ozge Can Ozcanli
// \date 10/01/08
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_update_changes_process : public bprb_process
{
 public:

  brec_update_changes_process();

  //: Copy Constructor (no local data)
  brec_update_changes_process(const brec_update_changes_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_update_changes_process(){};

  //: Clone the process
  virtual brec_update_changes_process* clone() const {return new brec_update_changes_process(*this);}

  vcl_string name(){return "brecUpdateChangesProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_update_changes_process_h_
