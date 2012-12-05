// This is brl/bseg/boxm2/pro/processes/boxm2_describe_scene_process.cxx
//:
// \file
// \brief  A process for loading the scene.
//
// \author Vishal Jain
// \date Mar 10, 2011

#include <bprb/bprb_func_process.h>

#include <vcl_fstream.h>
#include <boxm2/boxm2_scene.h>
#include <boxm2/boxm2_util.h>
#include <boxm2/boxm2_data_traits.h>

namespace boxm2_describe_scene_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 2;
}

bool boxm2_describe_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm2_describe_scene_process_globals;

  //process takes 1 input
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm2_scene_sptr";

  // process has 2 outputs:
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "vcl_string";                    //path to model data
  output_types_[1] = "vcl_string";                    //appearance model type
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm2_describe_scene_process(bprb_func_process& pro)
{
  using namespace boxm2_describe_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  //get the inputs
  boxm2_scene_sptr scene = pro.get_input<boxm2_scene_sptr>(0);
  if (!scene){
    vcl_cout << " null scene in boxm2_describe_scene_process\n";
    return false;
  }
  //vcl_cout << *scene;

  //verifies that a scene has a valid appearance, spits out data type and appearance type size
  vcl_vector<vcl_string> valid_types;
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG3_GREY_16>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_GAUSS_RGB>::prefix());
  valid_types.push_back(boxm2_data_traits<BOXM2_MOG6_VIEW>::prefix());
  vcl_string data_type;
  int appTypeSize;
  boxm2_util::verify_appearance(*scene, valid_types, data_type, appTypeSize);
  vcl_cout<<"DATA_TYPE:"<<data_type<<vcl_endl;

  //set model dir as output
  vcl_string dataPath = scene->data_path();
  pro.set_output_val<vcl_string>(0, dataPath);
  pro.set_output_val<vcl_string>(1, data_type);
  return true;
}
