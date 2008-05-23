// This is brl/bseg/bvxm/pro/bvxm_ocp_compare_process.h
#ifndef bvxm_ocp_compare_process_h_
#define bvxm_ocp_compare_process_h_
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
#include <bvxm/bvxm_voxel_world.h>

class bvxm_ocp_compare_process : public bprb_process
{
 public:

   bvxm_ocp_compare_process();

  //: Copy Constructor (no local data)
  bvxm_ocp_compare_process(const bvxm_ocp_compare_process& other)
  : bprb_process(*static_cast<const bprb_process*>(&other)) { }

  ~bvxm_ocp_compare_process(){}

  //: Clone the process
  virtual bvxm_ocp_compare_process* clone() const
  { return new bvxm_ocp_compare_process(*this); }

  vcl_string name() { return "bvxmOcpCompareProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
private:
  double compare(bvxm_voxel_world_sptr w1, bvxm_voxel_world_sptr w2);
  bool save_raw(char *ocp_array, int x, int y, int z, vcl_string filename);
};

#endif // bvxm_ocp_compare_process_h_
