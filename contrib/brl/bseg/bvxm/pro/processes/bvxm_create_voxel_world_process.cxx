//This is brl/bseg/bvxm/pro/processes/bvxm_create_voxel_world_process.cxx
#include <string>
#include <iostream>
#include <fstream>
#include "bvxm_create_voxel_world_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_world_params.h>

#include <brdb/brdb_value.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

//:sets input and output types
bool bvxm_create_voxel_world_process_cons(bprb_func_process& pro)
{
  //set output types
  using namespace bvxm_create_voxel_world_process_globals;
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_world_sptr";
  return pro.set_output_types(output_types_);
}

//:creates a voxel world
bool bvxm_create_voxel_world_process(bprb_func_process& pro)
{
  using namespace bvxm_create_voxel_world_process_globals;
  //define and read in the parameters
  std::string vox_dir;
  pro.parameters()->get_value(param_input_directory_, vox_dir);
  if (!vul_file::is_directory(vox_dir) || !vul_file::exists(vox_dir)) {
    std::cerr << "In bvxm_create_voxel_world_process::execute() -- input directory "  << vox_dir << "is not valid!\n";
    return false;
  }

  //std::cout << "In bvxm_create_voxel_world_process::execute() -- input directory is: "  << vox_dir << std::endl;



  float corner_x = 0.0f;
  pro.parameters()->get_value(param_corner_x_, corner_x);
  float corner_y = 0.0f;
  pro.parameters()->get_value(param_corner_y_, corner_y);
  float corner_z = 0.0f;
  pro.parameters()->get_value(param_corner_z_, corner_z);
  vgl_point_3d<float> corner(corner_x, corner_y, corner_z);

  unsigned int dimx = 10;
  pro.parameters()->get_value(param_voxel_dim_x_, dimx);
  unsigned int dimy = 10;
  pro.parameters()->get_value(param_voxel_dim_y_, dimy);
  unsigned int dimz = 10;
  pro.parameters()->get_value(param_voxel_dim_z_, dimz);
  vgl_vector_3d<unsigned int> voxel_dims(dimx, dimy, dimz);

  float vox_len = 1.0f;
  pro.parameters()->get_value(param_voxel_length_, vox_len);

  std::string lvcs_path;
  pro.parameters()->get_value(param_lvcs_, lvcs_path);

  vpgl_lvcs_sptr lvcs = new vpgl_lvcs();
  if (lvcs_path != "") {
    std::ifstream is(lvcs_path.c_str());
    if (!is)
    {
      std::cerr << " Error opening file  " << lvcs_path << std::endl;
      return false;
    }
    lvcs->read(is);
  }

  float min_ocp_prob = 1e-5f;
  pro.parameters()->get_value(param_min_ocp_prob_, min_ocp_prob);
  float max_ocp_prob = 1- 1e-5f;
  pro.parameters()->get_value(param_max_ocp_prob_, max_ocp_prob);

  unsigned int max_scale = 1;
  pro.parameters()->get_value<unsigned int>(param_max_scale_, max_scale);

  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(vox_dir, corner, voxel_dims, vox_len, lvcs, min_ocp_prob, max_ocp_prob, max_scale);

  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world;
  vox_world->set_params(params);

  //store output
  pro.set_output_val<bvxm_voxel_world_sptr>(0, vox_world);

  return true;
}
