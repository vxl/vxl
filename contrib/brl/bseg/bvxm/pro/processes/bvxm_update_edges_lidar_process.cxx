// This is brl/bseg/bvxm/pro/processes/bvxm_update_edges_lidar_process.cxx
#include "bvxm_update_edges_lidar_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vil/vil_image_view_base.h>
#include <vpgl/vpgl_camera_double_sptr.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_mog_grey_processor.h>

bool bvxm_update_edges_lidar_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_lidar_process_globals;
  //process takes 4inputs
  //input[0]: The lidar edge height image
  //input[1]: The lidar edge prob image
  //input[2]: The camera of the observation (dummy)
  //input[3]: The voxel world
  //input[4]: scale index
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vil_image_view_base_sptr";
  input_types_[1] = "vil_image_view_base_sptr";
  input_types_[2] = "vil_image_view_base_sptr";
  input_types_[3] = "vpgl_camera_double_sptr";
  input_types_[4] = "bvxm_voxel_world_sptr";
  input_types_[5] = "unsigned";

  return pro.set_input_types(input_types_);
}

bool bvxm_update_edges_lidar_process(bprb_func_process& pro)
{
  using namespace bvxm_update_edges_lidar_process_globals;

  // check number of inputs
  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  vil_image_view_base_sptr lidar_height = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr lidar_edges = pro.get_input<vil_image_view_base_sptr>(i++);
  vil_image_view_base_sptr lidar_edges_prob = pro.get_input<vil_image_view_base_sptr>(i++);
  vpgl_camera_double_sptr camera = pro.get_input<vpgl_camera_double_sptr>(i++);
  bvxm_voxel_world_sptr world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  auto scale_idx = pro.get_input<unsigned>(i++);

  if ( !lidar_height ){
    std::cout << pro.name() << " :--  Input 0  is not valid!\n";
    return false;
  }

  if ( !lidar_edges ){
    std::cout << pro.name() << " :--  Input 1  is not valid!\n";
    return false;
  }

  if ( !lidar_edges_prob ){
    std::cout << pro.name() << " :--  Input 2  is not valid!\n";
    return false;
  }

  if ( !camera ){
    std::cout << pro.name() << " :--  Input 3  is not valid!\n";
    return false;
  }

  bool result = true;

  for (unsigned curr_scale=scale_idx;curr_scale<world->get_params()->max_scale();curr_scale++)
  {
    result = result && world->update_edges_lidar(lidar_height,lidar_edges,lidar_edges_prob,camera,curr_scale);
  }

  if (!result){
    std::cerr << "error bvxm_update_edges_lidar_process: failed to update observation\n";
    return false;
  }

  return true;
}
