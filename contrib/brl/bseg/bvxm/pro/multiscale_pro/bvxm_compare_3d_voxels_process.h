// This is brl/bseg/bvxm/pro/multiscale_pro/bvxm_compare_3d_voxels_process.h
#ifndef bvxm_compare_3d_voxels_process_h_
#define bvxm_compare_3d_voxels_process_h_
//:
// \file
//
// \author Vishal Jain
// \date 04/15/2008
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_compare_3d_voxels_process : public bprb_process
{
 public:

  bvxm_compare_3d_voxels_process();

  //: Copy Constructor (no local data)
  bvxm_compare_3d_voxels_process(const bvxm_compare_3d_voxels_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){};

  ~bvxm_compare_3d_voxels_process(){};

  //: Clone the process
  virtual bvxm_compare_3d_voxels_process* clone() const {return new bvxm_compare_3d_voxels_process(*this);}

  vcl_string name(){return "bvxmCompare3DVoxelsProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};


#endif // bvxm_compare_3d_voxels_process_h_
