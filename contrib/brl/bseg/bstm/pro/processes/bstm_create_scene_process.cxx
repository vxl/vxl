// This is brl/bseg/bstm/pro/processes/bstm_create_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//

#include <vcl_fstream.h>
#include <vul/vul_file.h>
#include <bstm/bstm_scene.h>

namespace bstm_create_scene_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 1;
}

bool bstm_create_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_create_scene_process_globals;

  //process takes 10 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "float"; // origin x
  input_types_[4] = "float"; // origin y
  input_types_[5] = "float"; // origin z

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_scene_sptr";


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_create_scene_process(bprb_func_process& pro)
{
  using namespace bstm_create_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  vcl_vector<vcl_string> appearance(1,"");
  unsigned i = 0;
  vcl_string datapath = pro.get_input<vcl_string>(i++);
  appearance[0]       = pro.get_input<vcl_string>(i++); //Appearance Model String
  vcl_string opt_app  = pro.get_input<vcl_string>(i++); //Occupancy Model String
  float origin_x      = pro.get_input<float>(i++);
  float origin_y      = pro.get_input<float>(i++);
  float origin_z      = pro.get_input<float>(i++);

  if (!vul_file::make_directory_path(datapath.c_str()))
    return false;
  bstm_scene_sptr scene =new bstm_scene(datapath,vgl_point_3d<double>(origin_x,origin_y,origin_z),2);
  scene->set_local_origin(vgl_point_3d<double>(origin_x,origin_y,origin_z));

  if(opt_app.length() > 0)
    appearance.push_back(opt_app);
  scene->set_appearances(appearance);

  i=0;  // store scene smart pointer
  pro.set_output_val<bstm_scene_sptr>(i++, scene);
  return true;
}


namespace bstm_write_scene_xml_process_globals
{
  const unsigned n_inputs_  =2;
  const unsigned n_outputs_ =0;
}

bool bstm_write_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace bstm_write_scene_xml_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  bool ok = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default arguments - default filename is "scene"
  brdb_value_sptr filename = new brdb_value_t<vcl_string>("scene");
  pro.set_input(1, filename);
  return ok;
}

bool bstm_write_scene_xml_process(bprb_func_process& pro)
{
  using namespace bstm_write_scene_xml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene=pro.get_input<bstm_scene_sptr>(i++);
  vcl_string    filename=pro.get_input<vcl_string>(i++);

  //set xml path to reflect fname
  vcl_string xmlPath = scene->xml_path();
  vcl_string xmlDir = vul_file::dirname(xmlPath);
  scene->set_xml_path(xmlDir + "/" + filename + ".xml");

  //make file and x_write to file
  vcl_ofstream ofile(scene->xml_path().c_str());
  x_write(ofile,(*scene.ptr()), "scene");
  return true;
}

