// This is brl/bseg/bvxm/rec/pro/brec_recognize_structure_process.h
#ifndef brec_recognize_structure_process_h_
#define brec_recognize_structure_process_h_
//:
// \file
// \brief A class to find instances of various structures, objects (e.g. vehicles) according to a part hierarchy constructed a priori
//
// \author Ozge Can Ozcanli
// \date Oct 28, 2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class brec_recognize_structure_process : public bprb_process
{
 public:

  brec_recognize_structure_process();

  //: Copy Constructor (no local data)
  brec_recognize_structure_process(const brec_recognize_structure_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~brec_recognize_structure_process(){};

  //: Clone the process
  virtual brec_recognize_structure_process* clone() const {return new brec_recognize_structure_process(*this);}

  vcl_string name() { return "brecRecStructureProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // brec_recognize_structure_process_h_
