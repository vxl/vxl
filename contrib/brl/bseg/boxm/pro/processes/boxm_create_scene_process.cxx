//This is brl/bseg/boxm/pro/processes/boxm_create_scene_process.cxx
#include <string>
#include <iostream>
#include <fstream>
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

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_parser.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/sample/boxm_inf_line_sample.h>
#include <boxm/sample/boxm_edge_tangent_sample.h>
#include <boxm/sample/boxm_scalar_sample.h>

//:global variables
namespace boxm_create_scene_process_globals
{
  //this process takes no inputs
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

//:sets input and output types
bool boxm_create_scene_process_cons(bprb_func_process& pro)
{
  using namespace boxm_create_scene_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//:creates a scene from parameters
bool boxm_create_scene_process(bprb_func_process& pro)
{
  using namespace boxm_create_scene_process_globals;
  std::string fname = pro.get_input<std::string>(0);

  boxm_scene_parser parser;

  boxm_scene_base_sptr scene_ptr=new boxm_scene_base();
  scene_ptr->load_scene(fname, parser);
  std::cout << "Scene path: " << scene_ptr->filename()<< '\n'
           << "Scene appearance: " <<scene_ptr->appearence_model()<< std::endl;

  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
    else {
      typedef boct_tree<short,boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      std::cout<<"Multi Bin set"<<std::endl;
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_MOB_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_MOB_GREY> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
    else {
      std::cout<<"Multi Bin for BOXM_APM_MOB_GREY is not defined yet"<<std::endl;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
#if 0
    else {
      typedef boct_tree<short,boxm_sample_multi_bin<BOXM_APM_SIMPLE_GREY> > tree_type;
      boxm_scene<tree_type>* scene = new boxm_scene<tree_type>();
      scene->load_scene(fname);
      std::cout<<"Multi Bin set"<<std::endl;
      scene_ptr = scene;
    }
#endif
  }
  else if (scene_ptr->appearence_model() == BOXM_FLOAT) {
      if (!scene_ptr->multi_bin())
      {
        typedef boct_tree<short,float> tree_type;
        auto* scene = new boxm_scene<tree_type>();
        scene->load_scene(fname);
        scene_ptr = scene;
        std::cout << "Scene path: " << scene->filename()<< std::endl;
      }
  }
  else if (scene_ptr->appearence_model() == VNL_FLOAT_3) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,vnl_vector_fixed<float, 3> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(fname);
      scene_ptr = scene;
      std::cout << "Loading scene of type vnl_float_3" << '\n'
               << "Scene path: " << scene->filename()<< std::endl;
    }
  }
  else if (scene_ptr->appearence_model() == VNL_FLOAT_10) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,vnl_vector_fixed<float, 10> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(fname);
      scene_ptr = scene;
      std::cout << "Loading scene of type vnl_float_10" << '\n'
               << "Scene path: " << scene->filename()<< std::endl;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_BOOL) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,bool> tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(fname);
      scene_ptr = scene;
      std::cout << "Scene path: " << scene->filename()<< std::endl;
    }
  }
  else if (scene_ptr->appearence_model() == BSTA_MOG_F1) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BSTA_GAUSS_F1) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,bsta_num_obs<bsta_gauss_sf1> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_EDGE_FLOAT) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_edge_sample<float> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_EDGE_LINE) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_inf_line_sample<float> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_EDGE_TANGENT_LINE) {
    if (!scene_ptr->multi_bin())
    {
      typedef boct_tree<short,boxm_edge_tangent_sample<float> > tree_type;
      auto* scene = new boxm_scene<tree_type>();
      scene->load_scene(parser);
      scene_ptr = scene;
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_SCALAR_FLOAT) {
    typedef boct_tree<short, boxm_scalar_sample<float> > tree_type;
    auto* scene = new boxm_scene<tree_type>();
    scene->load_scene(parser);
    scene_ptr = scene;
  }
  else {
    std::cerr << "boxm_create_scene_process: undefined APM type\n";
    return false;
  }

  //store output
  pro.set_output_val<boxm_scene_base_sptr>(0, scene_ptr);

  return true;
}
