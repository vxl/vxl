// This is brl/bseg/boxm/algo/pro/processes/boxm_save_scene_raw_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for writing general scenes to drishti raw files. Values in voxels are saved as they are
//
// \author Isabel Restrepo
// \date Feb 17, 2011
// \verbatim
//  Modifications
//  <None>
// \endverbatim

#include <vcl_fstream.h>
#include <vcl_sstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_save_scene_raw_general.h>
#include <boxm/boxm_apm_traits.h>

namespace boxm_save_scene_raw_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm_save_scene_raw_process_cons(bprb_func_process& pro)
{
  using namespace boxm_save_scene_raw_process_globals;
  
  // process takes 4 inputs but has no outputs
  //input[0]: scene binary file
  //input[1]: output file (raw) path
  //input[2]: the resolution level wanted.. 0 is the most detailed (lowest possible level)
  //input[3]: 0 for writing blocks individually, 1 for writing the scene into one file
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_save_scene_raw_process(bprb_func_process& pro)
{
  using namespace boxm_save_scene_raw_process_globals;
  
  if ( pro.n_inputs() < n_inputs_) {
    vcl_cout << pro.name() << ": The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  
  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  vcl_string filepath = pro.get_input<vcl_string>(i++);
  unsigned resolution =  pro.get_input<unsigned>(i++);
  unsigned whole = pro.get_input<unsigned>(i++);
  
  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_FLOAT)
  {
    if (scene_ptr->multi_bin())
    {
      vcl_cout << "boxm_save_scene_raw_process: Multibin scenes not implemented" << vcl_endl;
      return false;
    }
    else
    {
      typedef boct_tree<short, float > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if(!scene)
        return false;
      if (!whole) {
        vcl_cout << "boxm_save_scene_raw_process: Save by block not implemented" << vcl_endl;
        return false;
      }
      else { // write the whole scene
        boxm_save_scene_raw_general_to_byte<short,float>(*scene, filepath + ".raw", resolution);
      }
    }
  }
  // check the scene's app model
  else if (scene_ptr->appearence_model() == BOXM_CHAR)
  {
    if (scene_ptr->multi_bin())
    {
      vcl_cout << "boxm_save_scene_raw_process: Multibin scenes not implemented" << vcl_endl;
      return false;
    }
    else
    {
      typedef boct_tree<short, char > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if(!scene)
        return false;
      if (!whole) {
        vcl_cout << "boxm_save_scene_raw_process: Save by block not implemented" << vcl_endl;
        return false;
      }
      else { // write the whole scene
        boxm_save_scene_raw_general<short,char>(*scene, filepath + ".raw", resolution);
      }
    }
  }
  // check the scene's app model
  else if (scene_ptr->appearence_model() == BOXM_BOOL)
  {
    if (scene_ptr->multi_bin())
    {
      vcl_cout << "boxm_save_scene_raw_process: Multibin scenes not implemented" << vcl_endl;
      return false;
    }
    else
    {
      typedef boct_tree<short, bool > type;
      boxm_scene<type>* scene = dynamic_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if(!scene)
        return false;
      if (!whole) {
        vcl_cout << "boxm_save_scene_raw_process: Save by block not implemented" << vcl_endl;
        return false;
      }
      else { // write the whole scene
        boxm_save_scene_raw_general<short>(*scene, filepath + ".raw", resolution);
      }
    }
  }  
  else {
    vcl_cout << "boxm_save_scene_raw_process: undefined APM type" << vcl_endl;
    return false;
  }
  return true;
}
