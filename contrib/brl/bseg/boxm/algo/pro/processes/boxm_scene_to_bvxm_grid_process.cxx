// This is brl/bseg/boxm/algo/boxm_scene_to_bvxm_grid_process.h

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

#include <boxm/algo/boxm_scene_to_bvxm_grid.h>
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>


namespace boxm_scene_to_bvxm_grid_process_globals
{
  const unsigned n_inputs_ = 4;
  const unsigned n_outputs_ = 1;
}

//: process takes 1 inputs and has no outputs.
// input[0]: The scene
// input[1]: Path to output grid
// input[2]: Resolution level
// input[3]: Bool to have full bvxm_grid in memory

// output[0]: The output grid
// outout[1]: The alpha scene


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
    vcl_cout << pro.name() << ": the input number should be " << n_inputs_
    << " but instead it is " << pro.n_inputs() << vcl_endl;
    return false;
  }
  
  //get inputs:
  boxm_scene_base_sptr scene_base = pro.get_input<boxm_scene_base_sptr>(0);
  vcl_string filepath = pro.get_input<vcl_string>(1);
  unsigned resolution_level = pro.get_input<short>(2);
  bool in_memory = pro.get_input<bool>(3);
  
  //check input's validity
  if (!scene_base.ptr()) 
  {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  
  if ( boxm_scene< boct_tree<short, float> > *scene= dynamic_cast<boxm_scene< boct_tree<short, float > > * >(scene_base.as_pointer()))
  {
    if(in_memory)
    {
      bvxm_voxel_grid<float> *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
      return true;
    }
    else
    {
      bvxm_voxel_grid<float> *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
      return true;
      
    }
  }
#if 0 // this case is not supported yet
  else if ( boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_f1> > > *scene= dynamic_cast<boxm_scene< boct_tree<short, bsta_num_obs<bsta_gauss_f1> > > * >(scene_base.as_pointer()))
  {
    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_f1> > *grid = boxm_scene_to_bvxm_grid(*scene, filepath, resolution_level);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;

  } 
#endif
  else
  {
    vcl_cerr << "It's not possible to convert input scene to a bvxm grid" << vcl_endl;
    return false;
  }
  
  return false;

}

