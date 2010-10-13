//This is brl/bseg/boxm/ocl/pro/processes/boxm_init_bit_scene_process.cxx
#include <boxm/ocl/boxm_ocl_bit_scene.h>
//:
// \file
// \brief Process for creating a new boxm_ocl_bit_scene 
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

#include <boxm/boxm_scene_parser.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_scalar_sample.h>

//:global variables
namespace boxm_init_bit_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}

//:sets input and output types
bool boxm_init_bit_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_init_bit_scene_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_ocl_bit_scene_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:creates a scene from parameters
bool boxm_init_bit_scene_process(bprb_func_process& pro)
{
  using namespace boxm_init_bit_scene_process_globals;
  vcl_string fname = pro.get_input<vcl_string>(0);

  //create boxm_ocl_bit_scene form xml file name
  boxm_ocl_bit_scene_sptr scene_ptr = new boxm_ocl_bit_scene(fname);
  
  vcl_cout<<(*(scene_ptr.as_pointer()))<<vcl_endl;
  scene_ptr.as_pointer()->validate_data();

  //store output
  pro.set_output_val<boxm_ocl_bit_scene_sptr>(0, scene_ptr);

  return true;
}
