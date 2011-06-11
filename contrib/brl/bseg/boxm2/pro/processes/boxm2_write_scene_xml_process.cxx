// This is brl/bseg/boxm2/pro/processes/boxm2_write_scene_xml_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for creating a scene
//
// \author Vishal Jain
// \date Mar 15, 2011

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>

namespace boxm2_write_scene_xml_process_globals
{
  const unsigned n_inputs_  =1;
  const unsigned n_outputs_ =0;
}

bool boxm2_write_scene_xml_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_write_scene_xml_process_globals;

  //process takes 2 inputs
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";

  // process has 1 output
  vcl_vector<vcl_string>  output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_write_scene_xml_process(bprb_func_process& pro)
{
  using namespace boxm2_write_scene_xml_process_globals;

  if ( pro.n_inputs() < n_inputs_ ) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  unsigned i = 0;
  boxm2_scene_sptr scene=pro.get_input<boxm2_scene_sptr>(i++);

  vcl_ofstream ofile(scene->xml_path().c_str());
  x_write(ofile,(*scene.ptr()),"scene");
  return true;
}
