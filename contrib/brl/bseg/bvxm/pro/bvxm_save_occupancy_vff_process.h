// This is brl/bseg/bvxm/pro/bvxm_save_occupancy_vff_process.h
#ifndef bvxm_save_occupancy_vff_process_h_
#define bvxm_save_occupancy_vff_process_h_
//:
// \file
// \brief Save the voxel world occupancy grid in binary format
// A process that saves the voxel world occupancy grid in a binary format
// readable by GE MicroView and other volume renderers
//
// \author Daniel Crispell
// \date 05/01/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_save_occupancy_vff_process : public bprb_process
{
 public:

   bvxm_save_occupancy_vff_process();

  //: Copy Constructor (no local data)
  bvxm_save_occupancy_vff_process(const bvxm_save_occupancy_vff_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_save_occupancy_vff_process(){};

  //: Clone the process
  virtual bvxm_save_occupancy_vff_process* clone() const {return new bvxm_save_occupancy_vff_process(*this);}

  vcl_string name(){return "bvxmSaveOccupancyVff";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_save_occupancy_vff_process_h_
