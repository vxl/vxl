// This is brl/bseg/boxm/algo/pro/processes/boxm_replace_const_app_process.cxx
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to replace the appearance model for the whole scene by a constant appearance model
// \author Vishal Jain
// \date July 28, 2009

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_refine.h>
#include <boxm/sample/boxm_sample.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/util/boxm_utils.h>

void boxm_replace_constant_app(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > &scene, float mean);

namespace boxm_replace_const_app_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm_replace_const_app_process_cons(bprb_func_process& pro)
{
  using namespace boxm_replace_const_app_process_globals;

  //process takes 2 inputs and no output
  //input[0]: The scene
  //input[1]: mean value for appearance
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_replace_const_app_process(bprb_func_process& pro)
{
  using namespace boxm_replace_const_app_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    std::cout << pro.name() << "boxm_replace_const_app_process: The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  auto meanval = pro.get_input<float>(i++);

  // check the input validity
  if (scene == nullptr) {
    std::cout << "boxm_replace_const_app_process: scene is null, cannot run" << std::endl;
    return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
    auto *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
    boxm_replace_constant_app(*s,meanval );
  }
  else {
    std::cout << "boxm_replace_const_app_process: undefined APM type" << std::endl;
    return false;
  }

  return true;
}

void boxm_replace_constant_app(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > &scene, float mean)
{
  boxm_block_iterator<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);
  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype app=boxm_utils::obtain_mog_grey_single_mode(mean);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* block = *iter;
    boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >* tree = block->get_tree();

    std::vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >*> leaf_cells=tree->leaf_cells();
    for (auto & leaf_cell : leaf_cells)
    {
      boxm_sample<BOXM_APM_MOG_GREY> new_data(leaf_cell->data().alpha,app);
      leaf_cell->set_data(new_data);
    }
    scene.write_active_block();
  }
}
