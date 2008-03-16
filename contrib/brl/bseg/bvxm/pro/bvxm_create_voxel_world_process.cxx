#include "bvxm_create_voxel_world_process.h"
//:
// \file
#include <bvxm/bvxm_voxel_world.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_world_params.h>

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>

//: Constructor
bvxm_create_voxel_world_process::bvxm_create_voxel_world_process()
{
  //this process takes no input

  //output
  output_data_.resize(1,brdb_value_sptr(0));
  output_types_.resize(1);
  output_types_[0]= "bvxm_voxel_world_sptr";

  // parameters
  // world parameters are received as process parameters

  if (!parameters()->add("Input Directory", "input_directory", vcl_string("./")))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Corner X", "corner_x", (float)0.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Corner Y", "corner_y", (float)0.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Corner Z", "corner_z", (float)0.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Dimension in X", "voxel_dim_x", (unsigned int)10))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Dimension in Y", "voxel_dim_y", (unsigned int)10))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Dimension in Z", "voxel_dim_z", (unsigned int)10))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Voxel Length", "voxel_length", (float)1.0))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  // path to the lvcs file
  if (!parameters()->add("LVCS Path", "lvcs", vcl_string("./")))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Minimum Occupancy Probability", "min_ocp_prob", 1e-5f))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;

  if (!parameters()->add("Maximum Occupancy Probability", "max_ocp_prob", 1-1e-5f))
    vcl_cerr << "ERROR: Adding parameters in " << __FILE__ << vcl_endl;
}

//: Destructor
bvxm_create_voxel_world_process::~bvxm_create_voxel_world_process()
{
}


//: Execute the process
bool
bvxm_create_voxel_world_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  vcl_string vox_dir;
  if (!parameters()->get_value(vcl_string("input_directory"), vox_dir)) {
    vcl_cout << "bvxm_create_voxel_world_process::execute() -- problem in retrieving parameter input_directory\n";
    return false;
  }

  if (!vul_file::is_directory(vox_dir) || !vul_file::exists(vox_dir)) {
    vcl_cerr << "In bvxm_create_voxel_world_process::execute() -- input directory is not valid!\n";
    return false;
  }

  vcl_cout << "In bvxm_create_voxel_world_process::execute() -- input directory is: " << vox_dir << vcl_endl;

  float corner_x = parameters()->value<float>("corner_x");
  float corner_y = parameters()->value<float>("corner_y");
  float corner_z = parameters()->value<float>("corner_z");
  vgl_point_3d<float> corner(corner_x, corner_y, corner_z);

  unsigned int dimx = parameters()->value<unsigned int>("voxel_dim_x");
  unsigned int dimy = parameters()->value<unsigned int>("voxel_dim_y");
  unsigned int dimz = parameters()->value<unsigned int>("voxel_dim_z");
  vgl_vector_3d<unsigned int> voxel_dims(dimx, dimy, dimz);

  float vox_len = parameters()->value<float>("voxel_length");

#if 0
  bvxm_world_params::appearance_model_type apm_type;
  unsigned int type = parameters()->value<unsigned int>("apm_type");
  if (type < 3) // there are 3 types and values are 0,1 and 2
    apm_type = (bvxm_world_params::appearance_model_type) type;
  else
    apm_type = bvxm_world_params::mog_grey;
#endif // 0

  vcl_string lvcs_path;
  if (!parameters()->get_value(vcl_string("lvcs"), lvcs_path)) {
    vcl_cout << "bvxm_create_voxel_world_process::execute() -- problem in retrieving parameter lvcs_path\n";
    return false;
  }

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

  float min_ocp_prob = parameters()->value<float>("min_ocp_prob");
  float max_ocp_prob = parameters()->value<float>("max_ocp_prob");

  bvxm_world_params_sptr params = new bvxm_world_params();
  params->set_params(vox_dir, corner, voxel_dims, vox_len, lvcs, min_ocp_prob, max_ocp_prob);

  bvxm_voxel_world_sptr vox_world = new bvxm_voxel_world;
  vox_world->set_params(params);

  brdb_value_sptr output0 = new brdb_value_t<bvxm_voxel_world_sptr>(vox_world);

  output_data_[0] = output0;

  return true;
}
