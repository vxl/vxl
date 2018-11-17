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
//   Yi Dong        - Mar 22, 2014 - add new method to output a unsigned short image with all 11 bits of the input NITF image kept
// \endverbatim

#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <bvxm/bvxm_world_params.h>
#include <vil/vil_image_view.h>
#include <vgl/vgl_box_3d.h>
#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

//: globals variables and functions
namespace bvxm_roi_init_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 3;

  // functions

  //: roi_init function (the most significant 5 bits and less significant 3 bits of the input 16 bits NITF image pixels will be ignored)
  bool roi_init(std::string const& image_path,
                vpgl_rational_camera<double>* camera,
                const bvxm_world_params_sptr& world_params,
                float uncertainty,
                vil_image_view<unsigned char>* nitf_image_unsigned_char,
                vpgl_local_rational_camera<double>& local_camera);

  //: roi init function to output a short image (the most significant 5 bits of the input 16 bits NITF image pixels will be ignored and all other 11 bits are kept)
  bool roi_init(std::string const& image_path,
                vpgl_rational_camera<double>* camera,
                const bvxm_world_params_sptr& world_params,
                float uncertainty,
                vil_image_view<vxl_uint_16>* nitf_image_unsigned_short,
                vpgl_local_rational_camera<double>& local_camera);

  //: projects the box on the image by taking the union of all the projected corners
  vgl_box_2d<double>* project_box(vpgl_rational_camera<double>* cam,
                                  const vpgl_lvcs_sptr& lvcs,
                                  vgl_box_3d<double> box,
                                  float r);
}

//: set input and output types
bool bvxm_roi_init_process_cons(bprb_func_process& pro);

bool bvxm_roi_init_process(bprb_func_process& pro);

#endif // bvxm_roi_init_process_h_
