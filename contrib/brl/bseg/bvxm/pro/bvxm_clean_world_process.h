// This is brl/bseg/bvxm/pro/bvxm_clean_world_process.h
#ifndef bvxm_clean_world_process_h_
#define bvxm_clean_world_process_h_
//:
// \file
// \brief A process that deletes all voxel storage files in the world directory. use with caution!
//
// \author Daniel Crispell
// \date 03/09/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_clean_world_process : public bprb_process
{
 public:

   bvxm_clean_world_process();

  //: Copy Constructor (no local data)
  bvxm_clean_world_process(const bvxm_clean_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_clean_world_process(){};

  //: Clone the process
  virtual bvxm_clean_world_process* clone() const {return new bvxm_clean_world_process(*this);}

  vcl_string name(){return "bvxmCleanWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_clean_world_process_h_
