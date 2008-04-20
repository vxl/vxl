// This is brl/bseg/bvxm/pro/bvxm_heightmap_process.h
#ifndef bvxm_heightmap_process_h_
#define bvxm_heightmap_process_h_
//:
// \file
// \brief A class for generating a height map from a given camera viewpoint
//        -  Input:
//             - bvxm_voxel_world_sptr
//             - vpgl_camera_double_sptr
//
//        -  Output:
//             - vil_image_view_base_sptr   generated image
//
// \author  Gamze D. Tunali
// \date    04/17/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <bvxm/bvxm_world_params.h>

class bvxm_heightmap_process : public bprb_process
{
 public:

   bvxm_heightmap_process();

  //: Copy Constructor (no local data)
  bvxm_heightmap_process(const bvxm_heightmap_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) { }

  ~bvxm_heightmap_process(){}

  //: Clone the process
  virtual bvxm_heightmap_process* clone() const
  { return new bvxm_heightmap_process(*this); }

  vcl_string name() { return "bvxmHeightmapProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // bvxm_heightmap_process_h_
