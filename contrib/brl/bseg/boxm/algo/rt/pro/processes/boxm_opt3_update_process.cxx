//This is brl/bseg/boxm/algo/rt/pro/processes/boxm_opt3_update_process.cxx
#include <string>
#include <iostream>
#include <fstream>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for updating a boxm_scene with a set of images. No damping parameter is required
// \author Daniel Crispell
// \date   February 17, 2010
//
// \verbatim
//  Modifications
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <boct/boct_tree.h>

#include <boxm/boxm_scene_base.h>
#include <boxm/boxm_scene.h>

#include <boxm/sample/boxm_opt3_sample.h>
#include <boxm/boxm_aux_traits.h>
#include <boxm/algo/rt/boxm_opt3_optimizer.h>

namespace boxm_opt3_update_process_globals
{
  constexpr unsigned int n_inputs_ = 2;
  constexpr unsigned int n_outputs_ = 0;
  //Define parameters here
}


//: set input and output types
bool boxm_opt3_update_process_cons(bprb_func_process& pro)
{
  using namespace boxm_opt3_update_process_globals;

  // process takes 2 inputs:
  //input[0]: The scene
  //input[1]: The filename of the text file containing list of image names

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";

  // process has 0 outputs:
  std::vector<std::string> output_types_(n_outputs_);

  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}

//: Execute the process
bool boxm_opt3_update_process(bprb_func_process& pro)
{
  using namespace boxm_opt3_update_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  // get the parameters
  // (none)

  // get the inputs
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  boxm_apm_type apm_type = scene_base->appearence_model();

  std::string image_list_fname = pro.get_input<std::string>(1);

  // extract list of image_ids from file
  std::ifstream ifs(image_list_fname.c_str());
  if (!ifs.good()) {
    std::cerr << "error opening file " << image_list_fname << '\n';
    return false;
  }
  std::vector<std::string> image_ids;
  unsigned int n_images = 0;
  ifs >> n_images;
  for (unsigned int i=0; i<n_images; ++i) {
    std::string img_id;
    ifs >> img_id;
    image_ids.push_back(img_id);
  }
  ifs.close();

  switch (apm_type)
  {
   case BOXM_APM_SIMPLE_GREY:
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_SIMPLE_GREY> > tree_type;
      auto *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
      if (!scene) {
        std::cerr << "error casting scene_base to scene\n";
        return false;
      }
      boxm_opt3_optimizer<short,BOXM_APM_SIMPLE_GREY,BOXM_APM_MOG_GREY> optimizer(*scene, image_ids);
      optimizer.optimize_cells();

      break;
    }
   case BOXM_APM_MOG_GREY:
    {
      typedef boct_tree<short,boxm_sample<BOXM_APM_MOG_GREY> > tree_type;
      auto *scene = dynamic_cast<boxm_scene<tree_type>*>(scene_base.ptr());
      if (!scene) {
        std::cerr << "error casting scene_base to scene\n";
        return false;
      }
      boxm_opt3_optimizer<short,BOXM_APM_MOG_GREY,BOXM_APM_MOG_GREY> optimizer(*scene, image_ids);
      optimizer.optimize_cells();

      break;
    }
#if 0 // commented out
   case BOXM_APM_SIMPLE_RGB:
    {
      boxm_scene<BOXM_APM_SIMPLE_RGB> *scene = dynamic_cast<boxm_scene<BOXM_APM_SIMPLE_RGB>*>(scene_base.ptr());
      if (!scene) {
        std::cerr << "error casting scene_base to scene\n";
        return false;
      }
      boxm_opt_rt_bayesian_optimizer<BOXM_APM_SIMPLE_RGB, BOXM_AUX_OPT_RT_RGB> optimizer(*scene, image_ids);
      optimizer.optimize_cells(damping_factor);

      break;
    }
#endif // 0
   default:
    std::cerr << "error - boxm_opt3_update_process: unsupported appearance model type " << apm_type << '\n';
    return false;
  }

  //store output
  // (none)

  return true;
}
