// This is brl/bseg/brec/pro/brec_change_area_process.h
#ifndef brec_change_area_process_h_
#define brec_change_area_process_h_
//:
// \file
// \brief A class to find expected area over the whole image for a given prob map
//
// \author Ozge Can Ozcanli
// \date Nov 06, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_change_area_process : public bprb_process
{
 public:

  brec_change_area_process();

  //: Copy Constructor (no local data)
  brec_change_area_process(const brec_change_area_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_change_area_process(){};

  //: Clone the process
  virtual brec_change_area_process* clone() const {return new brec_change_area_process(*this);}

  vcl_string name(){return "brecChangeAreaProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_change_area_process_h_
