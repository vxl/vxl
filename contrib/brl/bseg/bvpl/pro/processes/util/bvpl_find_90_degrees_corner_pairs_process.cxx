// This is brl/bseg/bvpl/pro/processes/util/bvpl_find_90_degrees_corner_pairs_process.cxx
#include <bvpl/util/bvpl_corner_pair_finder.h>
//:
// \file
// \brief A process to find corner pairs that are rotated 90 degrees
// \author Isabel Restrepo mir@lems.brown.edu
// \date  September 27, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>

namespace bvpl_find_corner_pairs_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;
}

//:
// Inputs
// * input[0]: The id grid
// * input[1]: The response grid
// * input[2]: A vector of searching kernels
// * input[3]: A vector with the corner kernel
// * input[4]: A path to save the grid with pairs
// Outputs:
// * output[0]: A vector containing the lines connecting corners found
// * output[1]: A grid containing the pairs (stored at the mid_point)

bool bvpl_find_corner_pairs_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "bvpl_kernel_vector_sptr";
  input_types_[3] = "bvpl_kernel_vector_sptr";
  input_types_[4] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_corner_pairs_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_find_corner_pairs_process(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
    bvxm_voxel_grid_base_sptr id_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  bvxm_voxel_grid_base_sptr response_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(1);
  bvpl_kernel_vector_sptr search_kernels = pro.get_input<bvpl_kernel_vector_sptr>(2);
  bvpl_kernel_vector_sptr corner_kernels = pro.get_input<bvpl_kernel_vector_sptr>(3);
  std::string out_path = pro.get_input<std::string>(4);

  std::cout << "No of Corner Kernels : " << corner_kernels->kernels_.size() << std::endl;
  if (!id_grid_base.ptr() || !response_grid_base.ptr() || !search_kernels.ptr() || !corner_kernels.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  //cast grid
  auto *id_grid = dynamic_cast<bvxm_voxel_grid<int>* > (id_grid_base.ptr());
  auto *response_grid = dynamic_cast<bvxm_voxel_grid<float>* > (response_grid_base.ptr());
  bvxm_voxel_grid<bvpl_pair> * out_grid= new bvxm_voxel_grid<bvpl_pair >(out_path, id_grid->grid_size());
  out_grid->initialize_data(bvpl_pair());

  bvpl_corner_pairs_sptr pairs = new bvpl_corner_pairs();

  bvpl_corner_pair_finder::find_pairs_no_lines(id_grid, response_grid, search_kernels, corner_kernels, out_grid);

  pro.set_output_val<bvpl_corner_pairs_sptr>(0, pairs);
  pro.set_output_val<bvxm_voxel_grid_base_sptr>(1,out_grid);
  return true;
}
