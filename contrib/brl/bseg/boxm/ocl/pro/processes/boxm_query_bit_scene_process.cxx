// This is brl/bseg/boxm/ocl/pro/processes/boxm_query_bit_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for initializing a n update bit  scene  manager using OpenCL 
//
// \author Vishal Jain
// \date Sept 29, 2010
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/ocl/boxm_ocl_bit_scene.h>
#include <boxm/ocl/boxm_update_bit_scene_manager.h>
#include <boxm/ocl/boxm_update_bit_scene_manager_sptr.h>
#include <bpro/core/bbas_pro/bbas_1d_array_float.h>

namespace boxm_query_bit_scene_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

bool boxm_query_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_query_bit_scene_process_globals;
  // process takes 3 inputs and has no outputs
  // input[0]: scene.xml file path
  // input[1]: prob_thresh
  // input[2]: output_directory
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_update_bit_scene_manager_sptr";
  input_types_[1] = "float";
  input_types_[2] = "float";
  input_types_[3] = "float";
  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string>  output_types_(n_outputs_);
  output_types_[0] = "bbas_1d_array_float_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

bool boxm_query_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_query_bit_scene_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << ": The number of inputs should be " << n_inputs_<< vcl_endl;
    return false;
  }

  // get the inputs
  unsigned i = 0;
  boxm_update_bit_scene_manager_sptr mgr     = pro.get_input<boxm_update_bit_scene_manager_sptr>(i++);
  float x=pro.get_input<float>(i++);
  float y=pro.get_input<float>(i++);
  float z=pro.get_input<float>(i++);

   mgr->query_point(vgl_point_3d<float>(x,y,z));
   mgr->read_output_debug();
   vcl_vector<float> data;
   mgr->get_output_debug_array(data);
   bbas_1d_array_float_sptr array1d=new bbas_1d_array_float(data.size());
   for (unsigned i=0;i<data.size();i++)
          array1d->data_array[i]=data[i];

   pro.set_output_val<bbas_1d_array_float_sptr>(0,array1d);
   return true;
}
