// This is brl/bseg/boxm/algo/pro/processes/boxm_merge_mixtures_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief process that merges all gaussian mixtures in a  boxm_scene into unimodal gaussians
//
// \author Isabel Restrepo
// \date December 8, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <boxm/algo/boxm_merge_mog.h>

#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_merge_mixtures_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}

//: process takes 1 input and 1 output.
// input[0]: The sample scene
// output[0]: The appearance scene

bool boxm_merge_mixtures_process_cons(bprb_func_process& pro)
{
  using namespace boxm_merge_mixtures_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "boxm_scene_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_merge_mixtures_process(bprb_func_process& pro)
{
  using namespace boxm_merge_mixtures_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);

  //check input's validity
  if (!scene_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  //:Note initial implementation is for fixed types, but this can be changed if more cases are needed
  typedef boxm_apm_traits<BOXM_APM_MOG_GREY>::apm_datatype apm_datatype;
  typedef boct_tree<short, apm_datatype> mog_tree_type;

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef boct_tree<short, gauss_type> gauss_tree_type;

  auto *mog_scene = dynamic_cast<boxm_scene<mog_tree_type>* > (scene_base.as_pointer());

  //check input's validity
  if (!mog_scene) {
    std::cout <<  " :-- Grid is not of valid type!\n";
    return false;
  }

  //parameters of the output scene are the same as those of the input scene
  boxm_scene<gauss_tree_type> *gauss_scene = new boxm_scene<gauss_tree_type>(mog_scene->lvcs(), mog_scene->origin(), mog_scene->block_dim(), mog_scene->world_dim());
  gauss_scene->set_paths(mog_scene->path(), "bsta_gauss_sf1");
  gauss_scene->set_appearance_model(BSTA_GAUSS_F1);

  boxm_merge_mog merger;
  merger.kl_merge_scene( *mog_scene, *gauss_scene);
  gauss_scene->write_scene("gaussf1_scene.xml");
  pro.set_output_val<boxm_scene_base_sptr>(0, gauss_scene);
  std::cout<<"Exiting" << std::endl;

  return true;
}
