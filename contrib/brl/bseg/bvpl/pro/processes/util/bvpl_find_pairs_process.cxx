// This is brl/bseg/bvpl/pro/processes/util/bvpl_find_pairs_process.cxx
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

namespace bvpl_find_pairs_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 2;
}

//:
// Inputs
// * input[0]: The pair grid
// * input[1]: The angle that separates features (90, 180)
// * input[2]: A vector of searching kernels
// * input[3]: A path to save the grid with pairs
// Outputs:
// * output[0]: A vector containing the lines connecting corners found
// * output[1]: A grid containing the pairs (stored at the mid_point)

bool bvpl_find_pairs_process_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_find_pairs_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "int";
  input_types_[2] = "bvpl_kernel_vector_sptr";
  input_types_[3] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_corner_pairs_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_find_pairs_process_process(bprb_func_process& pro)
{
  using namespace bvpl_find_pairs_process_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  bvxm_voxel_grid_base_sptr pair_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  int opposite_angle = pro.get_input<int>(1);
  bvpl_kernel_vector_sptr search_kernels = pro.get_input<bvpl_kernel_vector_sptr>(2);
  std::string out_path = pro.get_input<std::string>(3);


  if (!pair_grid_base.ptr() || !search_kernels.ptr()) {
    std::cout <<  " :-- One of the inputs is invalid\n";
    return false;
  }
  //cast grid
  auto *pair_grid = dynamic_cast<bvxm_voxel_grid<bvpl_pair>* > (pair_grid_base.ptr());
  bvxm_voxel_grid<bvpl_pair> * out_grid= new bvxm_voxel_grid<bvpl_pair >(out_path, pair_grid->grid_size());
  out_grid->initialize_data(bvpl_pair());

  bvpl_corner_pairs_sptr pairs = new bvpl_corner_pairs();
  bvpl_corner_pair_finder::find_pairs_no_lines(pair_grid, search_kernels, out_grid, opposite_angle);

  pro.set_output_val<bvpl_corner_pairs_sptr>(0, pairs);
  pro.set_output_val<bvxm_voxel_grid_base_sptr>(1,out_grid);
  return true;
}
