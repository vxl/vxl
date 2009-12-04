//This is brl/bseg/boxm/pro/processes/boxm_create_scene_process.cxx
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
//:
// \file
// \brief A process for creating a new boxm_scene when there is not a saved one.
//        It receives the parameters from a parameter file in XML
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

#include <boxm/boxm_sample_multi_bin.h>
#include <boxm/boxm_scene_parser.h>

//:global variables
namespace boxm_create_scene_process_globals
{
  //this process takes no inputs
  const unsigned n_inputs_ = 1;
  const unsigned n_outputs_ = 1;
}

//:sets input and output types
bool boxm_create_scene_process_cons(bprb_func_process& pro)
{
  //set output types
  using namespace boxm_create_scene_process_globals;

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
bool boxm_create_scene_process(bprb_func_process& pro)
{
  using namespace boxm_create_scene_process_globals;
  vcl_string fname = pro.get_input<vcl_string>(0);

  boxm_scene_parser parser;

  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(fname, parser);
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
    else {
      typedef boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      vcl_cout<<"Multi Bin set"<<vcl_endl;
      scene_ptr = scene;
    }
  } else if (scene_ptr->appearence_model() == BOXM_APM_MOB_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
    else {
      vcl_cout<<"Multi Bin for BOXM_APM_MOB_GREY is not defined yet "<<vcl_endl;
    }
  } else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
#if 0
    else {
      typedef boct_tree<short,boxm_sample_multi_bin<BOXM_APM_SIMPLE_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      vcl_cout<<"Multi Bin set"<<vcl_endl;
      scene_ptr = scene;
    }
#endif // 0
  }
  else {
    vcl_cout << "boxm_create_scene_process: undefined APM type" << vcl_endl;
    return false;
  }

  //store output
  pro.set_output_val<boxm_scene_base_sptr>(0, scene_ptr);

  return true;
}
