// This is brl/bseg/boxm2/pro/processes/boxm2_scene_vrml_process.cxx
//:
// \file
// \brief A process for writing scene blocks into a vrml file to visualize the 3d scene block structure (using local coords)
//
// \author Yi Dong
// \date August 01, 2013

#include <bprb/bprb_func_process.h>
#include <bvrml/bvrml_write.h>
#include <boxm2/boxm2_scene.h>
#include <vpgl/vpgl_lvcs.h>
#include <vgl/vgl_box_3d.h>
#include <vcl_iostream.h>

namespace boxm2_scene_vrml_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm2_scene_vrml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_scene_vrml_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_scene_vrml_process(bprb_func_process& pro)
{
  using namespace boxm2_scene_vrml_process_globals;
  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_ << vcl_endl;
    return false;
  }

  // get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  vcl_string vrml_name = pro.get_input<vcl_string>(1);
  vcl_ofstream ofs(vrml_name.c_str());
  bvrml_write::write_vrml_header(ofs);

  vcl_map<boxm2_block_id, boxm2_block_metadata> blks = scene->blocks();
  
  // obtain the largest id along z direction
  unsigned max_z_id = 0;
  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator mit = blks.begin(); mit != blks.end(); ++mit)
    if (max_z_id < mit->first.k())
      max_z_id = mit->first.k();

  for (vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator mit = blks.begin(); mit != blks.end(); ++mit) {
    vgl_box_3d<double> box = mit->second.bbox();
    boxm2_block_id id = mit->first;
    float transparency = 0;
    float r = (float)id.k()/(float)max_z_id*255;
    float g = 0;
    float b = 0;
    bvrml_write::write_vrml_box(ofs, box, r, g, b, transparency);
  }
  vcl_cout << " write " << scene->num_blocks() << " are written in file " << vrml_name << vcl_endl;
  ofs.close();
  return true;
}