#ifndef bvxm_create_multiscale_voxel_world_process_h_
#define bvxm_create_multiscale_voxel_world_process_h_

//:
// \file
// \brief A process for creating a new bvxm_voxel_world from scratch



#include <vcl_string.h>
#include <bprb/bprb_process.h>

class bvxm_create_multiscale_voxel_world_process : public bprb_process
{
 public:
  
  bvxm_create_multiscale_voxel_world_process();

  //: Copy Constructor (no local data)
  bvxm_create_multiscale_voxel_world_process(const bvxm_create_multiscale_voxel_world_process& other): bprb_process(*static_cast<const bprb_process*>(&other)){}

 ~bvxm_create_multiscale_voxel_world_process();

  //: Clone the process
  virtual bvxm_create_multiscale_voxel_world_process* clone() const {return new bvxm_create_multiscale_voxel_world_process(*this);}

  vcl_string name(){return "bvxmCreateMultiScaleVoxelWorldProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
 
 private:

};

#endif //bvxm_create_multiscale_voxel_world_process_h_
