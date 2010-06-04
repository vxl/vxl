// This is brl/bseg/bvxm/algo/pro/processes/boxm_scene_to_bvxm_grid_process.cxx
#include <bvxm/algo/bvxm_boxm_convert.h>
//:
// \file
// \brief  Process to convert a boxm_scene to a bvxm_voxel_grid
// \author Isabel Restrepo mir@lems.brown.edu
// \date  Jan 19, 2010
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_scene_to_bvxm_grid_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

//: process takes 4 inputs and 1 output.
// input[0]: The scene
// input[1]: Path to output grid
// input[2]: Resolution level
// input[3]: Bool: Enforce only cells at the resolution leve?
//           If true, there is no interpolation
// output[0]: The output grid

bool boxm_scene_to_bvxm_grid_process_cons(bprb_func_process& pro)
{
  using namespace boxm_scene_to_bvxm_grid_process_globals;

  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "boxm_scene_base_sptr";
  input_types_[1] = "vcl_string";
  input_types_[2] = "unsigned";
  input_types_[3] = "bool";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool boxm_scene_to_bvxm_grid_process(bprb_func_process& pro)
{
  using namespace boxm_scene_to_bvxm_grid_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cerr << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << vcl_endl;
    return false;
  }

  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string filepath = pro.get_input<vcl_string>(1);
  unsigned resolution_level = pro.get_input<short>(2);
  bool enforce_level = pro.get_input<bool>(3);

  //check input's validity
  if (!scene_base.ptr())
  {
    vcl_cerr << pro.name() << ": -- Input grid is not valid!\n";
    return false;
  }

  if ( boxm_scene< boct_tree<short, float> > *scene= dynamic_cast<boxm_scene< boct_tree<short, float > > * >(scene_base.as_pointer()))
  {
    bvxm_voxel_grid<float> *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level,enforce_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }
  else if ( scene_base->appearence_model() == BSTA_GAUSS_F1)
  {
    boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_f1> > > *scene= dynamic_cast<boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_f1> > > * >(scene_base.as_pointer());
    if (!scene)
      vcl_cerr << "what's going on\n";

    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> > *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level, enforce_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }

  else
  {
    vcl_cerr << "It's not possible to convert input scene to a bvxm grid\n";
    return false;
  }

  return false;
}

