// This is brl/bseg/bvxm/pro/processes/bvxm_roi_init_process.h
#ifndef bvxm_roi_init_process_h_
#define bvxm_roi_init_process_h_
//:
// \file
// \brief A class for clipping and image based on a 3D bounding box.
//        -  Inputs:
//             * Image path (string)
//             * bvxm_voxel_world_sptr
//
//        -  Outputs:
//             * modified rational camera "vpgl_camera_double_sptr"
//             * clipped image area (NITF) "vil_image_view_base_sptr"
//
//        -  Params:
//             * geographic uncertainty (error) in meters
//
// \author  Gamze D. Tunali
// \date    Feb 19, 2008
// \verbatim
//  Modifications
//   Brandon Mayer  - Jan 28, 2009 - converted process-class to function to conform with new bvxm_process architecture.
//   Peter Vanroose - Jul 10, 2009 - split into .h and .cxx
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <vcl_string.h>

#include <bvxm/bvxm_world_params.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: globals variables and functions
namespace bvxm_roi_init_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 3;

  // functions

  //: roi_init function
  bool roi_init(vcl_string const& image_path,
                vpgl_rational_camera<double>* camera,
                bvxm_world_params_sptr world_params,
                float uncertainty,
                vil_image_view<unsigned char>* nitf_image_unsigned_char,
                vpgl_local_rational_camera<double>& local_camera);

  //: projects the box on the image by taking the union of all the projected corners
  vgl_box_2d<double>* project_box(vpgl_rational_camera<double>* cam,
                                  vpgl_lvcs_sptr lvcs,
                                  vgl_box_3d<double> box,
                                  float r);
}

//: set input and output types
bool bvxm_roi_init_process_cons(bprb_func_process& pro);

bool bvxm_roi_init_process(bprb_func_process& pro);

#endif // bvxm_roi_init_process_h_
