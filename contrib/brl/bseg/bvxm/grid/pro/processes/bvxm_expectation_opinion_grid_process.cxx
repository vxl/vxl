// This is brl/bseg/bvxm/grid/pro/processes/bvxm_expectation_opinion_grid_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for computing expectation of opinion from opinion grid
// \author Vishal Jain
// \date July  9, 2009
//
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/bvxm_voxel_grid_opinion_basic_ops.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

namespace bvxm_expectation_opinion_grid_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvxm_expectation_opinion_grid_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_expectation_opinion_grid_process_globals;

  // process takes 3 inputs and has 1 output.
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0]="bvxm_voxel_grid_base_sptr";
  input_types_[1]="vcl_string"; //: output path for output grid
  input_types_[2]="vcl_string"; //: type of output path (expectation/belief)

  // No outputs to the database. The resulting grid is stored on disk
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]="bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}


//: Execute the process
bool bvxm_expectation_opinion_grid_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 3)
  {
    std::cout << pro.name() << "The number of inputs should be 2" << std::endl;
    return false;
  }

  unsigned i=0;
  bvxm_voxel_grid_base_sptr input_grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  std::string output_path= pro.get_input<std::string>(i++);
  std::string output_type= pro.get_input<std::string>(i++);

  if (auto * opinion_input_grid=dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(input_grid.ptr()))
  {
    bvxm_voxel_grid<float> * grid_out=new bvxm_voxel_grid<float>(output_path, opinion_input_grid->grid_size());

    if (output_type=="expectation")
      bvxm_expectation_opinion_voxel_grid(opinion_input_grid, grid_out);
    else if (output_type=="belief")
      bvxm_belief_opinion_voxel_grid(opinion_input_grid, grid_out);
    else
    {
      std::cout<<"Type Unexpected"<<std::endl;
      return false;
    }
    std::cout<<"Expectation/Belief computed done."<<std::endl;
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
  }
  else
  {
    std::cout<<"Error! Input grid type is Wrong!"<<std::endl;
  }
  return true;
}
