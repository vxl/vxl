// This is contrib/bvxm/pro/bvxm_compare_3d_voxels_process.h
#ifndef bvxm_compare_3d_voxels_process_h_
#define bvxm_compare_3d_voxels_process_h_
//:
// \file
//
//
//  
//
// \author Vishal Jain
// \date 04/15/2008
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
