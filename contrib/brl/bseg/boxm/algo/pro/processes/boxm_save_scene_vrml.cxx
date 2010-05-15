// This is brl/bseg/boxm/algo/pro/processes/boxm_save_scene_vrml.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief process saves a boxm_scene as to a vrml file
//
// \author Isabel Restrepo
// \date December 9, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/algo/boxm_vrml_util.h>

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_save_scene_vrml_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

//: process takes 2 inputs and has no outputs.
// input[0]: The scene
// input[1]: Path to output vrml file

bool boxm_save_scene_vrml_process_cons(bprb_func_process& pro)
{
  using namespace boxm_save_scene_vrml_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";

  vcl_vector<vcl_string> output_types_(n_outputs_);

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_save_scene_vrml_process(bprb_func_process& pro)
{
  using namespace boxm_save_scene_vrml_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << vcl_endl;
    return false;
  }

  // get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string vrml_path = pro.get_input<vcl_string>(1);

  // check input's validity
  if (!scene_base.ptr()) {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  // Note initial implementation is for fixed types, but this can be changed if more cases are needed

  typedef boct_tree<short, float > tree_type;
  boxm_scene<tree_type> *scene_in = static_cast<boxm_scene<tree_type>* > (scene_base.as_pointer());
  vcl_ofstream ofs(vrml_path.c_str());
  boxm_vrml_util::write_vrml_scene(ofs, scene_in);

  return true;
}


