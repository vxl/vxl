// This is brl/bseg/boxm2/pro/processes/boxm2_transform_model_process.cxx
//:
// \file
// \brief  A process for applying a similarity transform to the model by modifying the parameters
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>
#include <vcl_map.h>
#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>
#include <boxm2/boxm2_block.h>
#include <boxm2/boxm2_block_metadata.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>
#include <vpgl/vpgl_lvcs.h>

namespace boxm2_transform_model_process_globals
{
  const unsigned n_inputs_ = 8;
  const unsigned n_outputs_ = 0;
}

bool boxm2_transform_model_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_transform_model_process_globals;

  //process takes 8 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "float"; // translation X
  input_types_[2] = "float"; // translation Y
  input_types_[3] = "float"; // translation Z
  input_types_[4] = "float"; // rotation about X axis (radians)
  input_types_[5] = "float"; // rotation about Y axis (radians)
  input_types_[6] = "float"; // rotation about Z axis (radians)
  input_types_[7] = "float"; // scaling

  // process has 0 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_)
      && pro.set_output_types(output_types_);
}

bool boxm2_transform_model_process(bprb_func_process& pro)
{
  using namespace boxm2_transform_model_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  if (!scene) {
    vcl_cout << " null scene in boxm2_transform_model_process\n";
    return false;
  }
  double translation_x = pro.get_input<float>(1);
  double translation_y = pro.get_input<float>(2);
  double translation_z = pro.get_input<float>(3);
  vgl_vector_3d<double> trans_vec(translation_x, translation_y, translation_z);
#if 0 // FIXME - currently unused!!
  double rotation_x = pro.get_input<float>(4);
  double rotation_y = pro.get_input<float>(5);
  double rotation_z = pro.get_input<float>(6);
  vgl_vector_3d<double> rot_vec(rotation_x, rotation_y, rotation_z);
  double scale = pro.get_input<float>(7);
#endif

#if 0
  vpgl_lvcs lvcs = scene->lvcs();
  double lox, loy, theta;
  lvcs.get_transform(lox,loy,theta);
  double lat,lon,elev;
  lvcs.get_origin(lat,lon,elev);
  lox += translation_x;
  loy += translation_y;
  elev += translation_z;
  lvcs.set_transform(lox,loy,theta);
  lvcs.set_origin(lat,lon,elev);

  scene->set_lvcs(lvcs);
#else
  vcl_map<boxm2_block_id, boxm2_block_metadata>& blocks = scene->blocks();
  vcl_map<boxm2_block_id, boxm2_block_metadata>::iterator bit = blocks.begin();
  for (; bit != blocks.end(); ++bit) {
    vgl_point_3d<double> old_origin = bit->second.local_origin_;
    bit->second.local_origin_ = old_origin + trans_vec;
  }
#endif

  return true;
}
