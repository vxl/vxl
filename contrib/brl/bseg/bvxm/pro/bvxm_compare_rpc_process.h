// This is brl/bseg/bvxm/pro/bvxm_compare_rpc_process.h
#ifndef bvxm_compare_rpc_process_h_
#define bvxm_compare_rpc_process_h_
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Input:
//             - Image path (string)
//             - bvxm_voxel_world_sptr
//
//        -  Output:
//             - modified rational camera "vpgl_camera_double_sptr"
//             - clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             -geographic uncertainty (error) in meters
//
// \author  Gamze D. Tunali
// \date    02/19/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>

#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>

class bvxm_compare_rpc_process : public bprb_process
{
 public:

   bvxm_compare_rpc_process();

  //: Copy Constructor (no local data)
  bvxm_compare_rpc_process(const bvxm_compare_rpc_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)) {}

  ~bvxm_compare_rpc_process() {}

  //: Clone the process
  virtual bvxm_compare_rpc_process* clone() const
    { return new bvxm_compare_rpc_process(*this); }

  vcl_string name() { return "bvxmCompareRpcProcess"; }

  bool init() { return true; }
  bool execute();
  bool finish() { return true; }
};

#endif // bvxm_compare_rpc_process_h_
