// This is brl/bseg/bvxm/pro/bvxm_roc_process.h
#ifndef bvxm_roc_process_h_
#define bvxm_roc_process_h_
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Isabel Restrepo
// \date 04/19/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bvxm_roc_process : public bprb_process
{
 public:

  bvxm_roc_process();

  //: Copy Constructor (no local data)
  bvxm_roc_process(const bvxm_roc_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_roc_process(){};

  //: Clone the process
  virtual bvxm_roc_process* clone() const {return new bvxm_roc_process(*this);}

  vcl_string name(){return "bvxmRocProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_roc_process_h_
