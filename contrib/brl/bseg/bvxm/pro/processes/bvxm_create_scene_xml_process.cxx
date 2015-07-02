// This is brl/bseg/bvxm/pro/processes/bvxm_create_scene_xml_process.cxx
#include "bvxm_create_scene_xml_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bvxm/bvxm_world_params.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vcl_string.h>
#include <vul/vul_file.h>

// set up input types
bool bvxm_create_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0]  = "vcl_string";      // scene xml output file
  input_types_[1]  = "vcl_string";      // scene world directory
  input_types_[2]  = "float";           // scene corner x coordinate
  input_types_[3]  = "float";           // scene corner y coordinate
  input_types_[4]  = "float";           // scene corner z coordinate
  input_types_[5]  = "unsigned";        // scene dimension in x direction
  input_types_[6]  = "unsigned";        // scene dimension in y direction
  input_types_[7]  = "unsigned";        // scene dimension in z direction
  input_types_[8]  = "float";           // scene voxel size
  input_types_[9]  = "vpgl_lvcs_sptr";  // scene lvcs
  input_types_[10]  = "vcl_string";     // scene lvcs file path
  input_types_[11] = "float";           // scene minimum occupancy probability
  input_types_[12] = "float";           // scene maximum occupancy probability
  input_types_[13] = "unsigned";        // scene maximum scale
  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

// execution
bool bvxm_create_scene_xml_process(bprb_func_process& pro)
{
  using namespace bvxm_create_scene_xml_process_globals;
  // sanity check
  if (!pro.verify_inputs()) {
    vcl_cerr << pro.name() << ": Wrong inputs!!\n";
    return false;
  }

  // get input
  unsigned in_i = 0;
  vcl_string  scene_xml = pro.get_input<vcl_string>(in_i++);
  vcl_string  world_dir = pro.get_input<vcl_string>(in_i++);
  float       corner_x  = pro.get_input<float>(in_i++);
  float       corner_y  = pro.get_input<float>(in_i++);
  float       corner_z  = pro.get_input<float>(in_i++);
  unsigned       dim_x  = pro.get_input<unsigned>(in_i++);
  unsigned       dim_y  = pro.get_input<unsigned>(in_i++);
  unsigned       dim_z  = pro.get_input<unsigned>(in_i++);
  float     voxel_size  = pro.get_input<float>(in_i++);
  vpgl_lvcs_sptr  lvcs  = pro.get_input<vpgl_lvcs_sptr>(in_i++);
  vcl_string lvcs_file  = pro.get_input<vcl_string>(in_i++);
  float   min_ocp_prob  = pro.get_input<float>(in_i++);
  float   max_ocp_prob  = pro.get_input<float>(in_i++);
  unsigned   max_scale  = pro.get_input<unsigned>(in_i++);

  if (!lvcs) {
    vcl_cerr << pro.name() << ": input scene lvcs is empty!!\n";
    return false;
  }
  if (!vul_file::exists(lvcs_file)) {
    vcl_cerr << pro.name() << ": can not find lvcs file - " << lvcs_file << "!!\n";
    return false;
  }

  // create scene parameter
  vgl_point_3d<float> corner(corner_x, corner_y, corner_z);
  vgl_vector_3d<unsigned> num_voxels(dim_x, dim_y, dim_z);

  bvxm_world_params params;
  params.set_params(world_dir, corner, num_voxels, voxel_size, lvcs, min_ocp_prob, max_ocp_prob, max_scale);
  params.write_xml(scene_xml, lvcs_file);

  return true;
}
