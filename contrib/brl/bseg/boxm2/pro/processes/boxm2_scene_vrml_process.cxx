// This is brl/bseg/boxm2/pro/processes/boxm2_scene_vrml_process.cxx
//:
// \file
// \brief A process for writing scene blocks into a vrml file to visualize the 3d scene block structure (using local coords)
//
// \author Yi Dong
// \date August 01, 2013

#include <iostream>
#include <bprb/bprb_func_process.h>
#include <bvrml/bvrml_write.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_compiler.h>

namespace boxm2_scene_vrml_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_scene_vrml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_vrml_process_globals;
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_vrml_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_vrml_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  std::string vrml_name = pro.get_input<std::string>(1);
  std::ofstream ofs(vrml_name.c_str());
  bvrml_write::write_vrml_header(ofs);

  std::map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();

  // obtain the largest id along z direction
  int max_z_id = 0;
  for (std::map<boxm2_block_id, boxm2_block_metadata>::iterator mit = blks.begin(); mit != blks.end(); ++mit)
    if (max_z_id < mit->first.k())
      max_z_id = mit->first.k();

  for (std::map<boxm2_block_id, boxm2_block_metadata>::iterator mit = blks.begin(); mit != blks.end(); ++mit) {
    vgl_box_3d<double> box = mit->second.bbox();
    boxm2_block_id id = mit->first;
    float transparency = 0;
    float r = (float)id.k()/(float)max_z_id*255;
    float g = 0;
    float b = 0;
    bvrml_write::write_vrml_box(ofs, box, r, g, b, transparency);
  }
  std::cout << " write " << scene->num_blocks() << " are written in file " << vrml_name << std::endl;
  ofs.close();
  return true;
}
