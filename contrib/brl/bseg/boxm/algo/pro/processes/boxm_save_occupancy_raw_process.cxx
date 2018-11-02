// This is brl/bseg/boxm/algo/pro/processes/boxm_save_occupancy_raw_process.cxx
#include <iostream>
#include <fstream>
#include <sstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for writing the occupancy probability of scenes to drishti raw files
//
// \author Gamze Tunali
// \date Apr 14, 2009
// \verbatim
//  Modifications
//   Sep 11, 2009   Gamze Tunali
//                  Added scene writing option, input number is increased to 4 and if 4th
//                  input is 0, blocks saved as raw files separately, if 1, then blocks
//                  combined to generate 1 raw file for the whole scene
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>
#include <boxm/algo/boxm_save_block_raw.h>
#include <boxm/algo/boxm_save_scene_raw.h>
#include <boxm/boxm_apm_traits.h>
#include <boxm/sample/boxm_sample_multi_bin.h>
#include <boxm/sample/boxm_scalar_sample.h>

namespace boxm_save_occupancy_raw_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

bool boxm_save_occupancy_raw_process_cons(bprb_func_process& pro)
{
  using namespace boxm_save_occupancy_raw_process_globals;

  // process takes 4 inputs but has no outputs
  //input[0]: scene binary file
  //input[1]: output file (raw) path
  //input[2]: the resolution level wanted.. 0 is the most detailed (lowest possible level)
  //input[3]: 0 for writing blocks individually, 1 for writing the scene into one file
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "unsigned";

  std::vector<std::string> output_types_(n_outputs_);
  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_save_occupancy_raw_process(bprb_func_process& pro)
{
  using namespace boxm_save_occupancy_raw_process_globals;

  if ( pro.n_inputs() < n_inputs_) {
    std::cout << pro.name() << ": The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get the inputs
  unsigned i = 0;
  boxm_scene_base_sptr scene_ptr = pro.get_input<boxm_scene_base_sptr>(i++);
  std::string filepath = pro.get_input<std::string>(i++);
  auto resolution =  pro.get_input<unsigned>(i++);
  auto whole = pro.get_input<unsigned>(i++);

  // check the scene's app model
  if (scene_ptr->appearence_model() == BOXM_APM_MOG_GREY)
  {
    if (scene_ptr->multi_bin())
    {
      typedef boct_tree<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> > type;
      auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          std::stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          std::string str(strm.str());
          std::string s = filepath + str + ".raw";
          boxm_save_block_raw<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      }
      else { // write the whole scene
        boxm_save_scene_raw<short, boxm_sample_multi_bin<BOXM_APM_MOG_GREY> >(*scene, filepath + ".raw", resolution);
      }
    }
    else
    {
      typedef boct_tree<short, boxm_sample<BOXM_APM_MOG_GREY> > type;
      auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
      if (!whole) {
        boxm_block_iterator<type> it(scene);
        it.begin();
        while (!it.end()) {
          std::stringstream strm;
          vgl_point_3d<int> index = it.index();
          strm << index.x() << '_' << index.y() << '_' << index.z();
          std::string str(strm.str());
          std::string s = filepath + str + ".raw";
          boxm_save_block_raw<short,boxm_sample<BOXM_APM_MOG_GREY> >(*scene, it.index(), s, resolution);
          it++;
        }
      }
      else { // write the whole scene
        boxm_save_scene_raw<short,boxm_sample<BOXM_APM_MOG_GREY> >(*scene, filepath + ".raw", resolution);
      }
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_SIMPLE_GREY) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_SIMPLE_GREY> > type;
    auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
    if (!whole) {
      boxm_block_iterator<type> it(scene);
      it.begin();
      while (!it.end()) {
        std::stringstream strm;
        vgl_point_3d<int> index = it.index();
        strm << index.x() << '_' << index.y() << '_' << index.z();
        std::string str(strm.str());
        std::string s = filepath + str + ".raw";
        boxm_save_block_raw<short,boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene, it.index(), s, resolution);
        it++;
      }
    }
    else { // write the whole scene
      boxm_save_scene_raw<short,boxm_sample<BOXM_APM_SIMPLE_GREY> >(*scene, filepath + ".raw", resolution);
    }
  }
  else if ( auto *scene= dynamic_cast<boxm_scene< boct_tree<short, float > > * >(scene_ptr.as_pointer())) {
    if (!whole) {
      boxm_block_iterator<boct_tree<short, float> > it(scene);
      it.begin();
      while (!it.end()) {
        std::stringstream strm;
        vgl_point_3d<int> index = it.index();
        strm << index.x() << '_' << index.y() << '_' << index.z();
        std::string str(strm.str());
        std::string s = filepath + str + ".raw";
        boxm_save_block_raw<short,float >(*scene, it.index(), s, resolution);
        it++;
      }
    }
    else { // write the whole scene
      boxm_save_scene_raw<short,float >(*scene, filepath + ".raw", resolution);
    }
  }
  else if (scene_ptr->appearence_model() == BSTA_MOG_F1){
    if (!whole) {
      typedef boct_tree<short,boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype > tree_type;
      auto *scene = dynamic_cast<boxm_scene<tree_type>* > (scene_ptr.as_pointer());
      boxm_block_iterator<tree_type> it(scene);
      it.begin();
      while (!it.end()) {
        std::stringstream strm;
        vgl_point_3d<int> index = it.index();
        strm << index.x() << '_' << index.y() << '_' << index.z();
        std::string str(strm.str());
        std::string s = filepath + str + ".raw";
        boxm_save_block_raw<short,boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype >(*scene, it.index(), s, resolution);
        it++;
      }
    }
    else { // write the whole scene
      //boxm_save_scene_raw<short,boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype >(*scene, filepath + ".raw", resolution);
    }
  }
  else if ( auto *scene= dynamic_cast<boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > * >(scene_ptr.as_pointer())) {
    if (!whole) {
      boxm_block_iterator<boct_tree<short, bsta_num_obs<bsta_gauss_sf1> > > it(scene);
      it.begin();
      while (!it.end()) {
        std::stringstream strm;
        vgl_point_3d<int> index = it.index();
        strm << index.x() << '_' << index.y() << '_' << index.z();
        std::string str(strm.str());
        std::string s = filepath + str + ".raw";
        boxm_save_block_raw<short,bsta_num_obs<bsta_gauss_sf1> >(*scene, it.index(), s, resolution);
        it++;
      }
    }
    else { // write the whole scene
      boxm_save_scene_raw<short,bsta_num_obs<bsta_gauss_sf1> >(*scene, filepath + ".raw", resolution);
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_APM_MOB_GREY) {
    typedef boct_tree<short, boxm_sample<BOXM_APM_MOB_GREY> > type;
    auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
    if (!whole) {
      boxm_block_iterator<type> it(scene);
      it.begin();
      while (!it.end()) {
        std::stringstream strm;
        vgl_point_3d<int> index = it.index();
        strm << index.x() << '_' << index.y() << '_' << index.z();
        std::string str(strm.str());
        std::string s = filepath + str + ".raw";
        boxm_save_block_raw<short,boxm_sample<BOXM_APM_MOB_GREY> >(*scene, it.index(), s, resolution);
        it++;
      }
    }
    else { // write the whole scene
      boxm_save_scene_raw<short,boxm_sample<BOXM_APM_MOB_GREY> >(*scene, filepath + ".raw", resolution);
    }
  }
  else if (scene_ptr->appearence_model() == BOXM_EDGE_FLOAT) {
    typedef boct_tree<short, boxm_edge_sample<float> > type;
    auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
    boxm_save_scene_raw<short,boxm_edge_sample<float> >(*scene, filepath + ".raw", resolution);
  }
  else if (scene_ptr->appearence_model() == BOXM_SCALAR_FLOAT) {
    typedef boct_tree<short, boxm_scalar_sample<float> > type;
    auto* scene = static_cast<boxm_scene<type>*>(scene_ptr.as_pointer());
    boxm_save_scene_raw<short,boxm_scalar_sample<float> >(*scene, filepath + ".raw", resolution);
  }
  else {
    std::cout << "boxm_save_occupancy_raw_process: undefined APM type" << std::endl;
    return false;
  }
  return true;
}
