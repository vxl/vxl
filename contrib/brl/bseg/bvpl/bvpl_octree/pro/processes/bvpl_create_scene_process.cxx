//This is brl/bseg/boxm/pro/processes/bvpl_create_scene_process.cxx
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process loading xml parameters of a boxm_scene of types specific to bvpl_octree
//      
//
// \author Isabel Restrepo
// \date Nov 15, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>


#include <vcl_string.h>
//#include <vcl_iostream.h>
//#include <vcl_fstream.h>

#include <boxm/boxm_scene.h>
#include <boxm/boxm_scene_parser.h>
#include <bvpl/bvpl_octree/bvpl_octree_sample.h>

//:global variables
namespace bvpl_create_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}

//:sets input and output types
bool bvpl_create_scene_process_cons(bprb_func_process& pro)
{
  //set output types
  using namespace bvpl_create_scene_process_globals;
  
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;
  
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  return true;
}

//:creates a scene from parameters
bool bvpl_create_scene_process(bprb_func_process& pro)
{
  using namespace bvpl_create_scene_process_globals;
  vcl_string fname = pro.get_input<vcl_string>(0);
  
  boxm_scene_parser parser;
  
  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(fname, parser);
  if (scene_ptr->appearence_model() == BVPL_SAMPLE_FLOAT) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,bvpl_octree_sample<float> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BVPL_SAMPLE_BSTA_GAUSS_F1) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,bvpl_octree_sample<bsta_num_obs<bsta_gauss_f1> > > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else {
    vcl_cout << "bvpl_create_scene_process: undefined datatype" << vcl_endl;
    return false;
  }
  
  //store output
  pro.set_output_val<boxm_scene_base_sptr>(0, scene_ptr);
  
  return true;
}
