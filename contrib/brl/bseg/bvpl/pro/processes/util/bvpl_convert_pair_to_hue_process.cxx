// This is brl/bseg/bvpl/pro/processes/util/bvpl_convert_pair_to_hue_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for converting a bvpl_pair to hue based on an id
// \author Isabel Restrepo
// \date October 5, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/util/bvpl_corner_pair_finder.h>
#include <vnl/vnl_float_4.h>

namespace bvpl_convert_pair_to_hue_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}


//: set input and output types
bool bvpl_convert_pair_to_hue_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_convert_pair_to_hue_process_globals;
  //This process has no inputs nor outputs only parameters
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="bvxm_voxel_grid_base_sptr"; //the input pair grid
  input_types_[i++]="bvpl_kernel_vector_sptr"; // a vector of kernels
  input_types_[i++]="vcl_string"; //path to output grid
  input_types_[i++]="vcl_string"; //output file for map

  if (!pro.set_input_types(input_types_))
    return false;
  std::vector<std::string> output_types_(n_outputs_);
  i=0;
  output_types_[i++]="bvxm_voxel_grid_base_sptr"; //the output grid
  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bvpl_convert_pair_to_hue_process(bprb_func_process& pro)
{
  using namespace bvpl_convert_pair_to_hue_process_globals;
  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    std::cout << pro.name() << "The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  bvxm_voxel_grid_base_sptr pair_base = pro.get_input<bvxm_voxel_grid_base_sptr>(0);
  bvpl_kernel_vector_sptr kernel_vector = pro.get_input<bvpl_kernel_vector_sptr>(1);
  std::string output_world_dir = pro.get_input<std::string>(2);
  std::string map_output_file = pro.get_input<std::string>(3);


  if ((!pair_base.ptr())) {
    std::cerr << "In bvpl_convert_pair_to_hue_process -- input grid is not valid!\n";
    return false;
  }
  if (auto *pair_grid = dynamic_cast< bvxm_voxel_grid<bvpl_pair >* >(pair_base.ptr()))
  {
    //assign hue values evenly dristributed on the color wheel
    //the wheel starts and ends on red, so we don't want to get back to the end
    std::vector<float> colors;
    float hue = 0.0f;
    for ( unsigned i = 0; i < kernel_vector->kernels_.size(); ++i){
      colors.push_back(hue);
      hue+=1.0f/float(kernel_vector->kernels_.size());
    }
    bvxm_voxel_grid<vnl_float_4 > * out_grid= new bvxm_voxel_grid<vnl_float_4 >(output_world_dir, pair_grid->grid_size());
    bvpl_convert_pair_grid_to_hsv_grid(pair_grid,out_grid, colors);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, out_grid);
   // bvpl_write_colors_to_svg(kernel_vector,colors,map_output_file);
    return true;
  }


  std::cerr << "datatype not supported\n";
  return false;
}
