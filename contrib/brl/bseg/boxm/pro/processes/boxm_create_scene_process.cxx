//This is brl/bseg/boxm/pro/processes/boxm_create_scene_process.cxx
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for creating a new boxm_scene when there is not a saved one.
//        It receives the parameters from a parameter file in XML
//
// \author Gamze Tunali
// \date Apr 21, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <vpgl/bgeo/bgeo_lvcs.h>
#include <vpgl/bgeo/bgeo_lvcs_sptr.h>

#include <vcl_string.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>

#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_vector_3d.h>

#include <vul/vul_file.h>


//:global variables
namespace boxm_create_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_outputs_ = 1;
  //Define parameters here
  const vcl_string param_input_directory_ =  "scene_path";
  const vcl_string param_apm_type_ =  "appearence_model";
  const vcl_string param_origin_x_ = "origin_x";
  const vcl_string param_origin_y_ = "origin_y";
  const vcl_string param_origin_z_ = "origin_z";
  const vcl_string param_block_dim_x_ = "block_dim_x";
  const vcl_string param_block_dim_y_ = "block_dim_y";
  const vcl_string param_block_dim_z_ = "block_dim_z";
  const vcl_string param_scene_dim_x_ = "scene_dim_x";
  const vcl_string param_scene_dim_y_ = "scene_dim_y";
  const vcl_string param_scene_dim_z_ = "scene_dim_z";
  const vcl_string param_lvcs_ = "lvcs";
  const vcl_string param_block_pref_ = "block_pref";
  const vcl_string param_min_ocp_prob_ = "min_ocp_prob";
  const vcl_string param_max_ocp_prob_ = "max_ocp_prob";
}

//:sets input and output types
bool boxm_create_scene_process_cons(bprb_func_process& pro)
{
  //set output types
  using namespace boxm_create_scene_process_globals;

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  return true;
}

//:creates a scene from parameters
bool boxm_create_scene_process(bprb_func_process& pro)
{
  using namespace boxm_create_scene_process_globals;

  //define and read in the parameters
  vcl_string scene_dir;
  pro.parameters()->get_value(param_input_directory_, scene_dir);
  if (!vul_file::is_directory(scene_dir) || !vul_file::exists(scene_dir)) {
    vcl_cerr << "In boxm_create_scene_process::execute() -- input directory "<< vul_file::get_cwd() << '/' << scene_dir << "is not valid!\n";
    return false;
  }
  vcl_cout << "In boxm_create_scene_process::execute() -- input directory is: " <<  vul_file::get_cwd() << scene_dir << vcl_endl;

  double origin_x = 0.0f;
  pro.parameters()->get_value(param_origin_x_, origin_x);
  double origin_y = 0.0f;
  pro.parameters()->get_value(param_origin_y_, origin_y);
  double origin_z = 0.0f;
  pro.parameters()->get_value(param_origin_z_, origin_z);
  vgl_point_3d<double> origin(origin_x, origin_y, origin_z);

  double dimx = 10;
  pro.parameters()->get_value(param_block_dim_x_, dimx);
  double dimy = 10;
  pro.parameters()->get_value(param_block_dim_y_, dimy);
  double dimz = 10;
  pro.parameters()->get_value(param_block_dim_z_, dimz);
  vgl_vector_3d<double> block_dims(dimx, dimy, dimz);

  double scene_dimx = 10;
  pro.parameters()->get_value(param_scene_dim_x_, scene_dimx);
  double scene_dimy = 10;
  pro.parameters()->get_value(param_scene_dim_y_, scene_dimy);
  double scene_dimz = 10;
  pro.parameters()->get_value(param_scene_dim_z_, scene_dimz);
  // world dimensions are computed from the block array dimensions which is given as paramaters
  vgl_vector_3d<double> scene_dims(scene_dimx*dimx, scene_dimy*dimy, scene_dimz*dimz);

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

  vcl_string block_pref;
  pro.parameters()->get_value(param_block_pref_, block_pref);

  float min_ocp_prob = 1e-5f;
  pro.parameters()->get_value(param_min_ocp_prob_, min_ocp_prob);
  float max_ocp_prob = 1- 1e-5f;
  pro.parameters()->get_value(param_max_ocp_prob_, max_ocp_prob);

  vcl_string apm_type;
  pro.parameters()->get_value(param_apm_type_, apm_type);

  boxm_scene_base_sptr scene=0;
  if (apm_type == "apm_mog_grey") {
    typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;

    scene = new boxm_scene<tree_type>(*lvcs, origin, block_dims, scene_dims);
  } else {
    vcl_cout << "boxm_create_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  //store output
  pro.set_output_val<boxm_scene_base_sptr>(0, scene);

  return true;
}
