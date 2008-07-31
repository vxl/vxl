// This is brl/bseg/bvxm/pro/bvxm_edges_lidar_init_process.h
#ifndef bvxm_edges_lidar_init_process_h_
#define bvxm_edges_lidar_init_process_h_
//:
// \file
// \brief A class for generating LiDAR edges given a LiDAR image pair.
//        -  Input:
//             - First return path (string)
//             - Second return path (string)
//
//        -  Output:
//             - clipped image area (first ret) "vil_image_view_base_sptr"
//             - clipped image area (second ret) "vil_image_view_base_sptr"
//             - mask "vil_image_view_base_sptr"
//
// \author  Ibrahim Eden
// \date    07/31/2008
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_string.h>
#include <vcl_vector.h>

#include <bgeo/bgeo_lvcs_sptr.h>
#include <bprb/bprb_process.h>
#include <vpgl/vpgl_local_rational_camera.h>
#include <vil/vil_image_view_base.h>
#include <vil/vil_image_resource_sptr.h>

#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_lidar_camera.h>

class bvxm_edges_lidar_init_process : public bprb_process
{
public:

  bvxm_edges_lidar_init_process();

  //: Copy Constructor (no local data)
  bvxm_edges_lidar_init_process(const bvxm_edges_lidar_init_process& other)
    : bprb_process(*static_cast<const bprb_process*>(&other)){}

  ~bvxm_edges_lidar_init_process(){}

  //: Clone the process
  virtual bvxm_edges_lidar_init_process* clone() const
  { return new bvxm_edges_lidar_init_process(*this); }

  vcl_string name(){return "bvxmEdgesLidarInitProcess";}

  bool init() { return true; }
  bool execute();
  bool finish(){return true;}
};

#endif // bvxm_edges_lidar_init_process_h_
