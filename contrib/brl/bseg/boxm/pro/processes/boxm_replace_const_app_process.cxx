// This is brl/bseg/boxm/pro/processes/boxm_replace_const_app_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief Process to replace the appearance model for the whole scene by a constant appearance model
// \author Vishal Jain
// \date July 28, 2009

#include <vcl_fstream.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_refine.h>
#include <boxm/boxm_sample.h>
#include <boxm/boxm_sample_multi_bin.h>
#include <boxm/boxm_utils.h>

void boxm_replace_constant_app(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > &scene,
                               float mean);
namespace boxm_replace_const_app_process_globals
{
  const unsigned n_inputs_ = 2;
  const unsigned n_outputs_ = 0;
}

bool boxm_replace_const_app_process_cons(bprb_func_process& pro)
{
  using namespace boxm_replace_const_app_process_globals;
  //process takes 2 inputs
  //input[0]: The scene
  //input[1]: mean value for appearance
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "float";
  if (!pro.set_input_types(input_types_))
    return false;

  //no output
  return true;
}

bool boxm_replace_const_app_process(bprb_func_process& pro)
{
  using namespace boxm_replace_const_app_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
    vcl_cout << pro.name() << "boxm_replace_const_app_process: The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  boxm_scene_base_sptr scene = pro.get_input<boxm_scene_base_sptr>(i++);
  float meanval = pro.get_input<float>(i++);

  // check the input validity
  if (scene == 0) {
     vcl_cout << "boxm_replace_const_app_process: scene is null, cannot run" << vcl_endl;
     return false;
  }

  if (scene->appearence_model() == BOXM_APM_MOG_GREY) {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      boxm_scene<tree_type> *s = static_cast<boxm_scene<tree_type>*> (scene.as_pointer());
      boxm_replace_constant_app(*s,meanval );
  }
  else {
    vcl_cout << "boxm_replace_const_app_process: undefined APM type" << vcl_endl;
    return false;
  }

  return true;
}

void boxm_replace_constant_app(boxm_scene<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > &scene,
                               float mean)
{
  boxm_block_iterator<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > > iter(&scene);
  boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype app=boxm_utils::obtain_mog_grey_single_mode(mean);
  for (; !iter.end(); iter++) {
    scene.load_block(iter.index());
    boxm_block<boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > >* block = *iter;
    boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> >* tree = block->get_tree();

    vcl_vector<boct_tree_cell<short,boxm_sample<BOXM_APM_MOG_GREY> >*> leaf_cells=tree->leaf_cells();
    for (unsigned i=0;i<leaf_cells.size();i++)
    {
      boxm_sample<BOXM_APM_MOG_GREY> new_data(leaf_cells[i]->data().alpha,app);
      leaf_cells[i]->set_data(new_data);
    }
    scene.write_active_block();
  }
}
