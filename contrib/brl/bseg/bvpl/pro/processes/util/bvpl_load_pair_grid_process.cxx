//This is brl/bseg/bvpl/pro/processes/util/bvpl_load_pair_grid_process.cxx

//:
// \file
// \brief A process loading and existing bvxm_voxel_grid<bvpl_pair>
// \author Isabel Restrepo
// \date June 15, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <iostream>
#include <string>
#include <bvpl/util/bvpl_corner_pair_finder.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <vul/vul_file.h>


namespace bvpl_load_pair_grid_process_globals
{
  constexpr unsigned n_inputs_ = 1;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvpl_load_pair_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_load_pair_grid_process_globals;

  //This process has no inputs nor outputs only parameters
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]="vcl_string"; //the input path

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";  // The resulting grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvpl_load_pair_grid_process(bprb_func_process& pro)
{
  using namespace bvpl_load_pair_grid_process_globals;

  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  std::string input_path = pro.get_input<std::string>(0);

  if (vul_file::is_directory(input_path) || !vul_file::exists(input_path)) {
    std::cerr << "In bvpl_load_pair_grid_process -- input path " << input_path<< "is not valid!\n";
    return false;
  }
  std::cout << "In bvpl_load_pair_grid_process( -- input file is: " <<  input_path << std::endl;

  bvxm_voxel_grid_base_sptr grid = new bvxm_voxel_grid<bvpl_pair>(input_path);

  if ( grid) {
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid);
    return true;
  }

  std::cerr << "In bvpl_load_pair_grid_process -- grid is not valid!\n";
  return false;
}
