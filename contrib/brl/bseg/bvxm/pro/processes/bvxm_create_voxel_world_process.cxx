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


//:global variables
namespace bvxm_create_voxel_world_process_globals
{
  //this process takes no inputs
  const unsigned n_outputs_ = 1; 
  //Define parameters here
  const vcl_string param_input_directory_ =  "input_directory";
  const vcl_string param_corner_x_ = "corner_x";
  const vcl_string param_corner_y_ = "corner_y";
  const vcl_string param_corner_z_ = "corner_z";
  const vcl_string param_voxel_dim_x_ = "voxel_dim_x";
  const vcl_string param_voxel_dim_y_ = "voxel_dim_y";
  const vcl_string param_voxel_dim_z_ = "voxel_dim_z";
  const vcl_string param_voxel_length_ = "voxel_length";
  const vcl_string param_lvcs_ = "lvcs";
  const vcl_string param_min_ocp_prob_ = "min_ocp_prob";
  const vcl_string param_max_ocp_prob_ = "max_ocp_prob";
  const vcl_string param_max_scale_ = "max_scale";
}

//:sets input and output types 
bool bvxm_create_voxel_world_process_init(bprb_func_process& pro)
{
  //set output types
  using namespace bvxm_create_voxel_world_process_globals;
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_world_sptr";
  if(!pro.set_output_types(output_types_))
    return false;
  return true;
  
}

//:creates a voxel world
bool bvxm_create_voxel_world_process(bprb_func_process& pro)
{
  using namespace bvxm_create_voxel_world_process_globals;
  //define and read in the parameters
  vcl_string vox_dir;
  pro.parameters()->get_value(param_input_directory_, vox_dir);
  if (!vul_file::is_directory(vox_dir) || !vul_file::exists(vox_dir)) {
    vcl_cerr << "In bvxm_create_voxel_world_process::execute() -- input directory "<< vul_file::get_cwd() << '/' << vox_dir << "is not valid!\n";
    return false;
  }
  vcl_cout << "In bvxm_create_voxel_world_process::execute() -- input directory is: " <<  vul_file::get_cwd() << vox_dir << vcl_endl;

  
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

  vcl_string lvcs_path;
  pro.parameters()->get_value(param_lvcs_, lvcs_path);

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
