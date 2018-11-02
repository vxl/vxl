// This is brl/bseg/boxm2/pro/processes/boxm2_write_scene_xml_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm2/boxm2_scene.h>
#include <vul/vul_file.h>

namespace boxm2_write_scene_xml_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm2_write_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_write_scene_xml_process_globals;

  //process takes 2 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";
  input_types_[1] = "vcl_string";

  // process has 1 output
  std::vector<std::string>  output_types_(n_outputs_);
  bool ok = pro.set_input_types(input_types_) && pro.set_output_types(output_types_);

  //default arguments - default filename is "scene"
  brdb_value_sptr filename = new brdb_value_t<std::string>("scene");
  pro.set_input(1, filename);
  return ok;
}

bool boxm2_write_scene_xml_process(bprb_func_process& pro)
{
  using namespace boxm2_write_scene_xml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene=pro.get_input<boxm2_scene_sptr>(i++);
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
