// This is brl/bseg/bvxm/pro/processes/bvxm_lidar_init_process.h
#ifndef bvxm_lidar_init_process_h_
#define bvxm_lidar_init_process_h_
//:
// \file
// \brief Functions  for clipping and image based on a 3D bounding box.
//        -  Inputs:
//             * First return path (string)
//             * Second return path (string)
//             * bvxm_voxel_world_sptr
//        -  Outputs:
//             * lidar pseudo camera "vpgl_camera_double_sptr"
//             * clipped image area (first ret) "vil_image_view_base_sptr"
//             * clipped image area (second ret) "vil_image_view_base_sptr"
//             * mask "vil_image_view_base_sptr"
//        -  Params:
//             * threshold for first and second return differences
//
// \author  Gamze D. Tunali
// \date    April 02, 2008
// \verbatim
//  Modifications
//   Isabel Restrepo - Jan 27, 2009 - converted process-class to functions which is the new design for bvxm_processes.
//   Peter Vanroose  - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>
#include <vcl_vector.h>

#include <bvxm/bvxm_world_params.h>
#include <vpgl/file_formats/vpgl_geo_camera.h>
#include <vnl/vnl_matrix.h>
#include <vil/vil_image_resource.h>
#include <vil/vil_image_view.h>

//: global variables/functions
namespace bvxm_lidar_init_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 4;

  // parameters identifying strings
  const vcl_string param_mask_thresh_ = "mask_thresh";

  // helper functions
  bool lidar_init(vil_image_resource_sptr lidar,
                  bvxm_world_params_sptr params,
                  vil_image_view_base_sptr& roi,
                  vpgl_geo_camera*& camera);

  bool comp_trans_matrix(double sx1, double sy1, double sz1,
                         vcl_vector<vcl_vector<double> > tiepoints,
                         vnl_matrix<double>& trans_matrix);

  bool gen_mask(vil_image_view_base_sptr roi_first,
                vpgl_geo_camera* cam_first,
                vil_image_view_base_sptr roi_second,
                vpgl_geo_camera* cam_second,
                vil_image_view_base_sptr& mask,
                double thresh);
}

//: set input and output types
bool bvxm_lidar_init_process_cons(bprb_func_process& pro);

bool bvxm_lidar_init_process(bprb_func_process& pro);

#endif // bvxm_lidar_init_process_h_
