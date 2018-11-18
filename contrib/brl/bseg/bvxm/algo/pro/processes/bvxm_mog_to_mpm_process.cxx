// This is brl/bseg/bvxm/algo/pro/processes/bvxm_mog_to_mpm_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process to conver a grid of mixture of gaussians to a grid with the most probable mode
//
// \author Isabel Restrepo mir@lems.brown.edu
//
// \date  8/17/09
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvxm/algo/bvxm_merge_mog.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bsta/bsta_gauss_sf1.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_mog_to_mpm_process_cons(bprb_func_process& pro)
{
  // Inputs
  // 0. Path to input grid(the one with gaussian mixtures)
  // 1. Path to univariate gaussian grid
  std::vector<std::string> input_types_(2);
  input_types_[0] = "vcl_string";
  input_types_[1] = "vcl_string";


  // No outputs to the database. The resulting grid is stored on disk
  std::vector<std::string> output_types_(0);

  if (!pro.set_input_types(input_types_))
    return false;

  return pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_mog_to_mpm_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 2)
  {
    std::cout << pro.name() << "The number of inputs should be " << 2 << std::endl;
    return false;
  }

  std::string apm_path = pro.get_input<std::string>(0);
  std::string output_path = pro.get_input<std::string>(1);

  //get the grids
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

  bvxm_voxel_grid_base_sptr apm_base = new bvxm_voxel_grid<mix_gauss_type>(apm_path);

  bvxm_voxel_grid_base_sptr output_base = new bvxm_voxel_grid<gauss_type>(output_path, apm_base->grid_size());

  //merge mixtures
  bvxm_merge_mog::mpm_grid(apm_base, output_base);

  return true;
}
