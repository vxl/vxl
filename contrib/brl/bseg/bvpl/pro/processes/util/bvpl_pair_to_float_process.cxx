//:
// \file
// \brief A process that saves a  bvxm_grid of bvpl_pairs to a binary format readable by the Drishti volume rendering program
//        (http://anusf.anu.edu.au/Vizlab/drishti/)
//
// \author Isabel Restrepo
// \date Oct 10, 2009
// \verbatim
//  Modifications

// \endverbatim


#include <bprb/bprb_func_process.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>

namespace bvpl_pair_to_float_process_globals
{
  constexpr unsigned n_inputs_ = 2;
  constexpr unsigned n_outputs_ = 1;
}

bool bvpl_pair_to_float_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_pair_to_float_process_globals;

  // process takes 2 inputs:
  //input[0]: The voxel_grid
  //input[1]: The filename to write to
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  return pro.set_output_types(output_types_)&& pro.set_input_types(input_types_);
}

bool bvpl_pair_to_float_process(bprb_func_process& pro)
{
  using namespace bvpl_pair_to_float_process_globals;

  // check number of inputs
  if (pro.n_inputs() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  std::string volume_path = pro.get_input<std::string>(1);

  // create the grid from in memory file and save
   if (auto *pair_grid = dynamic_cast< bvxm_voxel_grid<bvpl_pair >* >(grid_base.ptr()))
   {
     bvxm_voxel_grid<float> *float_grid = new bvxm_voxel_grid<float>(volume_path, pair_grid->grid_size());
     float_grid->initialize_data(0.0f);
     bvpl_convert_pair_grid_to_float_grid(pair_grid,float_grid);
     pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, float_grid);
     return true;
   }
   else
    std::cerr << "Grid type not supportted yet, but you can add one!\n";
  return false;
}
