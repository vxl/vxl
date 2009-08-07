// This is brl/bseg/bvxm/pro/processes/bvxm_update_edges_process.cxx
#include "bvxm_update_edges_process.h"
//:
// \file
// \brief A process that updates voxel world edge probabilities
// \author Ibrahim Eden
// \date Feb 11, 2009

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/bvxm_edge_util.h>
#include <bvxm/bvxm_edge_ray_processor.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>

#include <vpgl/vpgl_rational_camera.h>
#include <vpgl/vpgl_local_rational_camera.h>

#include <brip/brip_vil_float_ops.h>

#include <vpgl/algo/vpgl_backproject.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vgl/vgl_plane_3d.h>

#include <vil/vil_load.h>
#include <vcl_cstdio.h>

//: set input and output types
bool bvxm_update_edges_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_process_globals;

  // process takes 5 inputs:
  //input[0]: The voxel world
  //input[1]: The current camera
  //input[2]: The current image
  //input[3]: n_normal
  //input[4]: Scale of the image
  vcl_vector<vcl_string> input_types_(n_inputs_);
  unsigned i = 0;
  input_types_[i++] = "bvxm_voxel_world_sptr";
  input_types_[i++] = "vpgl_camera_double_sptr";
  input_types_[i++] = "vil_image_view_base_sptr";
  input_types_[i++] = "float";
  input_types_[i++] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has 1 outputs:
  // output[0]: New n_normal
  vcl_vector<vcl_string> output_types_(n_outputs_);
  unsigned j = 0;
  output_types_[j++] = "float";
  return pro.set_output_types(output_types_);
}

//:  optimizes rpc camera parameters based on edges
bool bvxm_update_edges_process(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_process_globals;

  //check number of inputs
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  // voxel world
  bvxm_voxel_world_sptr vox_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  // camera
  vpgl_camera_double_sptr camera_inp = pro.get_input<vpgl_camera_double_sptr>(i++);
  // image
  vil_image_view_base_sptr edge_image_sptr = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view<vxl_byte> edge_image(edge_image_sptr);

  float n_normal = pro.get_input<float>(i++);
  // scale of image
  unsigned scale = pro.get_input<unsigned>(i++);

  // get parameters
  int edge_prob_mask_size = 21;
  pro.parameters()->get_value(param_edge_prob_mask_size_, edge_prob_mask_size);
  float edge_prob_mask_sigma = 1.0f;
  pro.parameters()->get_value(param_edge_prob_mask_sigma_, edge_prob_mask_sigma);

  int num_observations = vox_world->num_observations<EDGES>(0,scale);

  vcl_cout << "number of observations before the update: " << num_observations << '\n'
           << "edge_prob_mask_size: " << edge_prob_mask_size << '\n'
           << "edge_prob_mask_sigma: " << edge_prob_mask_sigma << '\n';

  float new_n_normal = n_normal;
  bvxm_edge_ray_processor edge_ray_proc(vox_world);
  vil_image_view<float> edge_prob_image;
  bvxm_edge_util::estimate_edge_prob_image(edge_image, edge_prob_image, edge_prob_mask_size, edge_prob_mask_sigma);
  float edge_prob_image_mean;
  vil_math_mean(edge_prob_image_mean,edge_prob_image,0);

  new_n_normal = n_normal + edge_prob_image_mean;

  vil_image_view_base_sptr edge_prob_image_sptr = new vil_image_view<float>(edge_prob_image);

  bvxm_image_metadata camera_metadata_out(edge_prob_image_sptr,camera_inp);
  bool result = edge_ray_proc.update_edges(camera_metadata_out,0);

  if (!result) {
    vcl_cerr << "error bvxm_rpc_registration: failed to update edge image\n";
    return false;
  }

  // output
  unsigned j = 0;
  // update n_normal value and store
  pro.set_output_val<float >(j++, new_n_normal);

  return true;
}
