// This is brl/bseg/bvxm/pro/processes/bvxm_expected_edge_image_process.cxx
#include <iostream>
#include <cstdio>
#include "bvxm_expected_edge_image_process.h"
//:
// \file

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_edge_ray_processor.h>
#include <brip/brip_vil_float_ops.h>
#include <sdet/sdet_img_edge.h>
#include <vpgl/algo/vpgl_backproject.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_expected_edge_image_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_expected_edge_image_process_globals;

  // process takes 4 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: ni
  //input[3]: nj
  //input[4]: Scale of the image

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";
  input_types_[1] = "vpgl_camera_double_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  input_types_[4] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 2 outputs:
  // output[0]: Expected edge image (probabilities between 0 and 1, float)
  // output[1]: Expected edge image (normalized for display purposes, vxl_byte)
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "vil_image_view_base_sptr";
  output_types_[1] = "vil_image_view_base_sptr";
  return pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_expected_edge_image_process(bprb_func_process& pro)
{
  using namespace bvxm_expected_edge_image_process_globals;

  //check number of inputs
  if ( !pro.verify_inputs()) {
    std::cout << pro.name() << " - invalid inputs " << std::endl;
    return false;
  }

  // get the inputs
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(0);
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(1);
  bvxm_edge_ray_processor edge_proc(vox_world);

  //vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(2);
  //unsigned ni = edge_image_sptr->ni();
  //unsigned nj = edge_image_sptr->nj();
  //vil_image_view<vxl_byte> edge_image(edge_image_sptr);
  auto ni = pro.get_input<unsigned>(2);
  auto nj = pro.get_input<unsigned>(3);

  // scale of image
  auto scale = pro.get_input<unsigned>(4);

  int num_observations = vox_world->num_observations<EDGES>(0,scale);
  std::cout << "Number of observations in curren edge world: " << num_observations << '\n';

  float n_normal = vox_world->get_params()->edges_n_normal();
  std::cout << "n_normal: " << n_normal << '\n';

  // render the expected edge image
  vil_image_view_base_sptr dummy_img;
  bvxm_image_metadata camera_metadata_inp(dummy_img,camera_inp);
  auto *img_eei_f = new vil_image_view<float>(ni,nj,1);
  vil_image_view_base_sptr img_eei_f_sptr = img_eei_f;

  edge_proc.expected_edge_image(camera_metadata_inp,img_eei_f_sptr,n_normal,scale);

  auto *img_eei_vb = new vil_image_view<vxl_byte>(ni,nj,1);
  brip_vil_float_ops::normalize_to_interval<float,vxl_byte>(*img_eei_f,*img_eei_vb,0.0f,255.0f);


  pro.set_output_val<vil_image_view_base_sptr>(0, img_eei_f);
  pro.set_output_val<vil_image_view_base_sptr>(1, img_eei_vb);

  return true;
}
