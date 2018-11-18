// This is brl/bseg/bstm/pro/processes/bstm_create_scene_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <bstm/bstm_scene.h>

namespace bstm_create_scene_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 1;
}

bool bstm_create_scene_process_cons(bprb_func_process& pro)
{
  using namespace bstm_create_scene_process_globals;

  //process takes 10 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";
  input_types_[3] = "float"; // origin x
  input_types_[4] = "float"; // origin y
  input_types_[5] = "float"; // origin z

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  output_types_[0] = "bstm_scene_sptr";


  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bstm_create_scene_process(bprb_func_process& pro)
{
  using namespace bstm_create_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  std::vector<std::string> appearance(1,"");
  unsigned i = 0;
  std::string datapath = pro.get_input<std::string>(i++);
  appearance[0] = pro.get_input<std::string>(i++); //Appearance Model String
  std::string opt_app = pro.get_input<std::string>(i++); //Occupancy Model String
  auto origin_x = pro.get_input<float>(i++);
  auto origin_y = pro.get_input<float>(i++);
  auto origin_z = pro.get_input<float>(i++);

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
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool bstm_write_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace bstm_write_scene_xml_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bstm_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  bool ok = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default arguments - default filename is "scene"
  brdb_value_sptr filename = new brdb_value_t<std::string>("scene");
  pro.set_input(1, filename);
  return ok;
}

bool bstm_write_scene_xml_process(bprb_func_process& pro)
{
  using namespace bstm_write_scene_xml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  bstm_scene_sptr scene=pro.get_input<bstm_scene_sptr>(i++);
  std::string    filename=pro.get_input<std::string>(i++);

  //set xml path to reflect fname
  std::string xmlPath = scene->xml_path();
  std::string xmlDir = vul_file::dirname(xmlPath);
  scene->set_xml_path(xmlDir + "/" + filename + ".xml");

  //make file and x_write to file
  std::ofstream ofile(scene->xml_path().c_str());
  x_write(ofile,(*scene.ptr()), "scene");
  return true;
}
