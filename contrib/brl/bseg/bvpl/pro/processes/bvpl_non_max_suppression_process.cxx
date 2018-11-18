// This is brl/bseg/bvpl/pro/processes/bvpl_non_max_suppression_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for performing non-maxima suppression on a voxel grid.
//
// \author Isabel Restrepo
// \date August 27, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/bvpl_vector_operator.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

namespace bvpl_non_max_suppression_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}

//: process takes 4 inputs and has 1 outputs.
// input[0]: The response grid
// input[1]: The id grid
// input[2]: The vector of kernels
// input[3]: Output grid path to hold suppressed response
// output[0]: Output grid
bool bvpl_non_max_suppression_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_non_max_suppression_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "bvpl_kernel_vector_sptr";
  input_types_[3] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_non_max_suppression_process(bprb_func_process& pro)
{
  using namespace bvpl_non_max_suppression_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvxm_voxel_grid_base_sptr id_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  std::string out_grid_path = pro.get_input<std::string>(i++);
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

  //check input's validity
  if (!grid_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  if ( !kernel ) {
    std::cout << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }

  auto *id_grid=dynamic_cast<bvxm_voxel_grid<int >* >(id_grid_base.ptr());

  if (auto *grid = dynamic_cast<bvxm_voxel_grid<float>* > (grid_base.ptr())) {
    bvxm_voxel_grid<float> *grid_out=new bvxm_voxel_grid<float>(out_grid_path, grid->grid_size());
    bvpl_vector_operator  vector_oper;
    //copy one grid into another:
    bvxm_voxel_grid_copy<float>(grid, grid_out);
    vector_oper.non_maxima_suppression(grid_out,id_grid, kernel);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto *grid = dynamic_cast<bvxm_voxel_grid<bvxm_opinion>* > (grid_base.ptr())){
    bvxm_voxel_grid<bvxm_opinion> *grid_out=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, grid->grid_size());
    //copy one grid into another:
    bvxm_voxel_grid_copy<bvxm_opinion> (grid, grid_out);
    bvpl_vector_operator vector_oper;
    vector_oper.non_maxima_suppression(grid_out, id_grid, kernel);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto* grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(grid_base.ptr())) {
    bvxm_voxel_grid<gauss_type> *grid_out= new bvxm_voxel_grid<gauss_type>(out_grid_path, grid->grid_size());
    //copy one grid into another:
    bvxm_voxel_grid_copy<gauss_type> (grid, grid_out);
    bvpl_vector_operator vector_oper;
    vector_oper.non_maxima_suppression(grid_out,id_grid, kernel);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else
    return false;

  return false;
}
