// This is brl/bseg/bvpl/pro/processes/boxm_split_scene_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief process that splits a boxm_scene of boxm samples into an appearance and occupancy scene
//
// \author Isabel Restrepo
// \date December 3, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/algo/boxm_split_sample.h>

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_split_scene_process_globals
{
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 0;
}

//: process takes 1 inputs and has no outputs.
// input[0]: The sample scene

// Note: The resulting scenes are stored in the same directory as the input scene
//       The apperance scenes are notreturned as outputs, because it is assumed that this 
//       process won't be called regularly.

bool boxm_split_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_split_scene_process_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_split_scene_process(bprb_func_process& pro)
{
  using namespace boxm_split_scene_process_globals;
  
  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << ": the input number should be " << n_inputs_
    << " but instead it is " << pro.n_inputs() << vcl_endl;
    return false;
  }
  
  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);

  //check input's validity
  if (!scene_base.ptr()) {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  
  //:Note initial implementation is for fixed types, but this can be changed if more cases are needed
  
  typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > sample_tree_type;
  
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
  typedef boct_tree<short, apm_datatype> apm_tree_type;
  
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::obs_mathtype alpha_type;
  typedef boct_tree<short, alpha_type> alpha_tree_type;
  
  boxm_scene<sample_tree_type> *scene_in = static_cast<boxm_scene<sample_tree_type>* > (scene_base.as_pointer());
    
  //parameters of the output scene are the same as thos of the input scene
  boxm_scene<apm_tree_type> apm_scene(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim());
  apm_scene.set_paths(scene_in->path(), "apm_mog_grey");
  boxm_scene<alpha_tree_type > alpha_scene(scene_in->lvcs(), scene_in->origin(), scene_in->block_dim(), scene_in->world_dim());
  alpha_scene.set_paths(scene_in->path(), "alpha");
  
  boxm_split_sample<BOXM_APM_MOG_GREY> splitter;
  
  splitter.split_scene( *scene_in, apm_scene, alpha_scene);
  
  apm_scene.write_scene("/apm_scene.xml");
  alpha_scene.write_scene("alpha_scene.xml");
    
  
  return true;
}


