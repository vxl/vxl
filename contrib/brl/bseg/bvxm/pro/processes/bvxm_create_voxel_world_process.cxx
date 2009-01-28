//This is brl/bseg/bvxm/pro/processes/bvxm_create_voxel_world_process.cxx
//:
// \file
// \brief A process for creating a new bvxm_voxel_world from scratch
// \author Daniel Crispell
// \date 02/06/2008
//
// \verbatim
//  Modifications
//    Ozge C Ozcanli  02/19/2008  removed input string and modified to read input directory 
//                                path as a parameter from an XML  should be modified
//                                to read all the parameters from the same XML file
//   
//    Gamze Tunali    02/24/2008  added the parameter list to the process. 
//                                It receives all the world parameters as process 
//                                parameters now
//
//   Isabel Restrepo - 1/27/09 - converted process-class to functions which is the new design for bvxm_processes. 
// \endverbatim



#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_world_params.h>

#include <brdb/brdb_value.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

//Define parameters here
#define PARAM_INPUT_DIRECTORY "input_diectory"
#define PARAM_CORNER_X "corner_x"
#define PARAM_CORNER_Y "corner_y"
#define PARAM_CORNER_Z "corner_z"
#define PARAM_VOXEL_DIM_X "voxel_dim_x"
#define PARAM_VOXEL_DIM_Y "voxel_dim_y"
#define PARAM_VOXEL_DIM_Z "voxel_dim_z"
#define PARAM_VOXEL_LENGTH "voxel_length"
#define PARAM_LVCS "lvcs"
#define PARAM_MIN_OCP_PROB "min_ocp_prob"
#define PARAM_MAX_OCP_PROB "max_ocp_prob"
#define PARAM_MAX_SCALE "max_scale"

//: Creates a voxel world
bool bvxm_create_voxel_world_process(bprb_func_process& pro)
{
  //this process takes no inputs
  
  //define and read in the parameters
  vcl_string vox_dir;
  pro.parameters()->get_value(PARAM_INPUT_DIRECTORY, vox_dir);
  if (!vul_file::is_directory(vox_dir) || !vul_file::exists(vox_dir)) {
    vcl_cerr << "In bvxm_create_voxel_world_process::execute() -- input directory is not valid!\n";
    return false;
  }
  vcl_cout << "In bvxm_create_voxel_world_process::execute() -- input directory is: " << vox_dir << vcl_endl;

  
  float corner_x = 0.0f;
  pro.parameters()->get_value(PARAM_CORNER_X, corner_x);
  float corner_y = 0.0f;
  pro.parameters()->get_value(PARAM_CORNER_Y, corner_y);
  float corner_z = 0.0f;
  pro.parameters()->get_value(PARAM_CORNER_Z, corner_z);
  vgl_point_3d<float> corner(corner_x, corner_y, corner_z);

  unsigned int dimx = 10;
  pro.parameters()->get_value(PARAM_VOXEL_DIM_X, dimx);
  unsigned int dimy = 10;
  pro.parameters()->get_value(PARAM_VOXEL_DIM_Y, dimy);
  unsigned int dimz = 10;
  pro.parameters()->get_value(PARAM_VOXEL_DIM_Z, dimz);
  vgl_vector_3d<unsigned int> voxel_dims(dimx, dimy, dimz);

  float vox_len = 1.0f;
  pro.parameters()->get_value(PARAM_VOXEL_LENGTH, vox_len);

#if 0
  bvxm_world_params::appearance_model_type apm_type;
  unsigned int type = parameters()->value<unsigned int>("apm_type");
  if (type < 3) // there are 3 types and values are 0,1 and 2
    apm_type = (bvxm_world_params::appearance_model_type) type;
  else
    apm_type = bvxm_world_params::mog_grey;
#endif // 0

  vcl_string lvcs_path;
  pro.parameters()->get_value(PARAM_LVCS, lvcs_path);

  bgeo_lvcs_sptr lvcs = new bgeo_lvcs();
  if (lvcs_path != "") {
    vcl_ifstream is(lvcs_path.c_str());
    if (!is)
    {
      vcl_cerr << " Error opening file  " << lvcs_path << vcl_endl;
      return false;
    }
    lvcs->read(is);
  }

  float min_ocp_prob = 1e-5f;
  pro.parameters()->get_value(PARAM_MIN_OCP_PROB, min_ocp_prob);
  float max_ocp_prob = 1- 1e-5f;
  pro.parameters()->get_value(PARAM_MAX_OCP_PROB, max_ocp_prob);

  unsigned int max_scale = 1;
  pro.parameters()->get_value<unsigned int>(PARAM_MAX_SCALE, max_scale);

  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(vox_dir, corner, voxel_dims, vox_len, lvcs, min_ocp_prob, max_ocp_prob, max_scale);

  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world;
  vox_world->set_params(params);

  //Set and store outputs
  int j = 0;
  vcl_vector<vcl_string> output_types_(2);
  output_types_[j++] = "bvxm_voxel_world_sptr";
  pro.set_output_types(output_types_);
  
  j=0;
  //: output voxel world
  brdb_value_sptr output0 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);
  
  pro.set_output(j++, output0);

  return true;
}
