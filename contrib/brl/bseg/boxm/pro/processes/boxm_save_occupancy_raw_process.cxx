// This is brl/bseg/boxm/pro/processes/boxm_save_occupancy_raw_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for obtaining roc curve from change detection results.
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//    Sep 11, 2009   Gamze Tunali 
//         Added scene writing option, input number is increased to 4 and if 4th
//         input is 0, blocks saved as raw files seperately, if 1, then blocks 
//         combined to generate 1 raw file for the whole scene
// \endverbatim

#include <vcl_fstream.h>
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/boxm_save_block_raw.h>
#include <boxm/boxm_save_scene_raw.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/boxm_sample_multi_bin.h>

namespace boxm_save_occupancy_raw_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 0;
}

bool boxm_save_occupancy_raw_process_cons(bprb_func_process& pro)
{
  using namespace boxm_save_occupancy_raw_process_globals;
  //process takes 4 inputs
  //input[0]: scene binary file
  //input[1]: output file (raw) path
  //input[2]: the resolution level wanted.. 0 is the most detailed (lowest possible level)
  //input[3]: 0 for writing blocks individually, 1 for writing the scene into one file
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";
  if (!pro.set_input_types(input_types_))
    return false;

  // process has no output
  return true;
}

bool boxm_save_occupancy_raw_process(bprb_func_process& pro)
{
  using namespace boxm_save_occupancy_raw_process_globals;

  if ( pro.n_inputs() < n_inputs_ ){
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
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY)
  {
    if (scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          vcl_stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          vcl_string str(strm.str());
          vcl_string s = filepath + str + ".raw";
          boxm_save_block_raw<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      } else { // write the whole scene
        boxm_save_scene_raw<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*scene, filepath + ".raw", resolution);
      }

    }
    else
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
      boxm_scene<type>* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          vcl_stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          vcl_string str(strm.str());
          vcl_string s = filepath + str + ".raw";
          boxm_save_block_raw<short,boxm_sample<BOXM_APM_MOG_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      } else { // write the whole scene
        boxm_save_scene_raw<short,boxm_sample<BOXM_APM_MOG_GREY> >(*scene, filepath + ".raw", resolution);
      }
    }
  } else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
      typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
      boxm_scene<type>* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          vcl_stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          vcl_string str(strm.str());
          vcl_string s = filepath + str + ".raw";
          boxm_save_block_raw<short,boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      } else { // write the whole scene
        boxm_save_scene_raw<short,boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene, filepath + ".raw", resolution);
      }
  }
   else if (scene_ptr->appearence_model() == BOXM_APM_MOB_GREY) {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > type;
      boxm_scene<type>* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          vcl_stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          vcl_string str(strm.str());
          vcl_string s = filepath + str + ".raw";
          boxm_save_block_raw<short,boxm_sample<BOXM_APM_MOB_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      } else { // write the whole scene
        boxm_save_scene_raw<short,boxm_sample<BOXM_APM_MOB_GREY> >(*scene, filepath + ".raw", resolution);
      }
  }
  else {
    vcl_cout << "boxm_save_occupancy_raw_process: undefined APM type" << vcl_endl;
    return false;
  }
  return true;
}
