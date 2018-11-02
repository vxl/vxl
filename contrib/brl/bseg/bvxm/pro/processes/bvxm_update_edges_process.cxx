// This is brl/bseg/bvxm/pro/processes/bvxm_update_edges_process.cxx
#include <iostream>
#include <cstdio>
#include "bvxm_update_edges_process.h"
//:
// \file
// \brief A process that updates voxel world edge probabilities
// \author Ibrahim Eden
// \date Feb 11, 2009

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_edge_ray_processor.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vpgl/vpgl_camera.h>

#include <sdet/sdet_img_edge.h>

#include <brip/brip_vil_float_ops.h>

#include <vil/vil_image_view.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_update_edges_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_process_globals;

  // this process takes 4 inputs and has no outputs
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current image
  //input[3]: Scale of the image
  //input[4]: edge_prob_mask_size
  //input[5]: edge_prob_mask_sigma
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "unsigned";
  input_types_[i++] = "int";
  input_types_[i++] = "float";
  return pro.set_input_types(input_types_);
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_update_edges_process(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_edge_ray_processor edge_ray_proc(vox_world);
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> edge_image(edge_image_sptr);

  // scale of image
  auto scale = pro.get_input<unsigned>(i++);

  // edge update parameters
  int edge_prob_mask_size = pro.get_input<int>(i++);
  auto edge_prob_mask_sigma = pro.get_input<float>(i++);

#if 0
  // get parameters
  int edge_prob_mask_size = 21;
  pro.parameters()->get_value(param_edge_prob_mask_size_, edge_prob_mask_size);
  float edge_prob_mask_sigma = 1.0f;
  pro.parameters()->get_value(param_edge_prob_mask_sigma_, edge_prob_mask_sigma);
#endif

  int num_observations = vox_world->num_observations<EDGES>(0,scale);

  std::cout << "number of observations before the update: " << num_observations << '\n'
           << "edge_prob_mask_size: " << edge_prob_mask_size << '\n'
           << "edge_prob_mask_sigma: " << edge_prob_mask_sigma << '\n';

  vil_image_view<float> edge_prob_image;
  sdet_img_edge::estimate_edge_prob_image(edge_image, edge_prob_image, edge_prob_mask_size, edge_prob_mask_sigma);
  vil_image_view<float> edge_stat_image;
  sdet_img_edge::convert_true_edge_prob_to_edge_statistics(edge_prob_image,edge_stat_image);

  vil_image_view_base_sptr edge_stat_image_sptr = new vil_image_view<float>(edge_stat_image);

  bvxm_image_metadata camera_metadata_out(edge_stat_image_sptr,camera_inp);
  bool result = edge_ray_proc.update_edges(camera_metadata_out,0);

  if (!result) {
    std::cerr << "error bvxm_rpc_registration: failed to update edge image\n";
    return false;
  }

  return true;
}
