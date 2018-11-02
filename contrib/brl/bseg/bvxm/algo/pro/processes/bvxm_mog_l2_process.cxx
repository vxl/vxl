// This is brl/bseg/bvxm/algo/pro/processes/bvxm_mog_l2_process.cxx
//:
// \file
// \brief A process for computing the l2 distance between mixtures of gaussians in a grid.
//        At creation time the reference mixture is the first mixture encountered in the grid.
// \author Isabel Restrepo
// \date March 11, 2009

#include <iostream>
#include <string>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_file.h>
#include <bprb/bprb_func_process.h>
#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/algo/bvxm_mog_norm.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>

//: set input and output types
bool bvxm_mog_l2_process_cons(bprb_func_process& pro)
{
  // Inputs
  // 0. Path to mog grid, filr must exist
  // 1. Path to mask grid, if file doesn't exist a "ones" mask is created
  // 2. Path to output grid, if file exists it gets overwritten

  std::vector<std::string> input_types_(3);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";
  input_types_[2] = "vcl_string";

  // No outputs to the database. The resulting grid is stored on disk
  std::vector<std::string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  return pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_mog_l2_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 3)
  {
    std::cout << pro.name() << "The number of inputs should be " << 3 << std::endl;
    return false;
  }

  std::string apm_path = pro.get_input<std::string>(0);
  std::string mask_path = pro.get_input<std::string>(1);
  std::string output_path = pro.get_input<std::string>(2);

 //get the grids

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;
  bvxm_voxel_grid_base_sptr apm_base = new bvxm_voxel_grid<mix_gauss_type>(apm_path);

  //giving a mask is optional, if mask path does not exist, then an all "ones" mask is created
  //the mask path should still be specified as an input

  bool mask_exists = false;
  if (vul_file::exists(mask_path))
    mask_exists = true;

  bvxm_voxel_grid<float> *mask_base = new bvxm_voxel_grid<float>(mask_path, apm_base->grid_size());
  if (!mask_exists){
    std::cout<< "Mask file does not exist, creating dummy mask\n";
    mask_base->initialize_data(float(1));
  }


  bvxm_voxel_grid_base_sptr output_base = new bvxm_voxel_grid<float>(output_path, apm_base->grid_size());

  //calculate distances
  bvxm_mog_norm<float>::mog_l2_grid(apm_base, mask_base, output_base,false);

  return true;
}
