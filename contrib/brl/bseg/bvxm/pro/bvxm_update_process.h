// This is brl/bseg/bvxm/pro/bvxm_update_process.h
#ifndef bvxm_update_process_h_
#define bvxm_update_process_h_
//:
// \file
// \brief A class for update process of a voxel world.
//
// \author Isabel Restrepo
// \date 01/30/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>


class bvxm_update_process : public bprb_process
{
 public:

   bvxm_update_process();

  //: Copy Constructor (no local data)
  bvxm_update_process(const bvxm_update_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_update_process(){};

  //: Clone the process
  virtual bvxm_update_process* clone() const {return new bvxm_update_process(*this);}

  vcl_string name(){return "bvxmUpdateProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_update_process_h_
