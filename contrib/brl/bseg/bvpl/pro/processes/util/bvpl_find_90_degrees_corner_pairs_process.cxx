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
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 1;
}

//:
// Inputs
// * input[0]: The id grid
// * input[1]: A vector of searching kernels
// * input[2]: A vector with the corner kernel
// Outputs:
// * output[0]: A vector containing the lines connecting corners found

bool bvpl_find_corner_pairs_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvpl_kernel_vector_sptr";
  input_types_[2] = "bvpl_kernel_vector_sptr";

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvpl_corner_pairs_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_find_corner_pairs_process(bprb_func_process& pro)
{
  using namespace bvpl_find_corner_pairs_globals;

  if (pro.n_inputs() != n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr id_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_vector_sptr search_kernels = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  bvpl_kernel_vector_sptr corner_kernels = pro.get_input<bvpl_kernel_vector_sptr>(i++);

  if (!id_grid_base.ptr() || !search_kernels.ptr() || !corner_kernels.ptr()) {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  //cast grid
  bvxm_voxel_grid<int> *id_grid = dynamic_cast<bvxm_voxel_grid<int>* > (id_grid_base.ptr());

  bvpl_corner_pairs_sptr pairs =
    bvpl_corner_pair_finder::find_pairs(id_grid, search_kernels, corner_kernels);

  pro.set_output_val<bvpl_corner_pairs_sptr>(0, pairs);
  return true;
}
