// This is brl/bseg/bvpl/pro/processes/bvpl_neighborhood_operator_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for applying a kernel on a voxel world.
//
// \author Gamze Tunali
// \date June 19, 2009
// \verbatim
//  Modifications
//  8/13/09 Isabel Restrepo: Added support for appearance grid of unimodal gaussians
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>

#include <bsta/bsta_gauss_sf1.h>

#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/functors/bvpl_edge2d_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/functors/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>

namespace bvpl_neighborhood_operator_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_neighborhood_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_neighborhood_operator_process_globals;
  //process takes 5 inputs
  //input[0]: The grid
  //input[1]: The kernel
  //input[2]: The occupancy type:
  //          -float
  //          -opinion
  //input[3]: The functor type
  //input[4]: Output grid path
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvpl_kernel_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_neighborhood_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_neighborhood_operator_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr input_grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_sptr kernel = pro.get_input<bvpl_kernel_sptr>(i++);
  std::string ocp_type = pro.get_input<std::string>(i++);
  std::string functor_name = pro.get_input<std::string>(i++);
  std::string out_grid_path = pro.get_input<std::string>(i++);

  //check input's validity
  if (!input_grid.ptr()) {
    std::cout << pro.name() << " :-- Grid is not valid!\n";
    return false;
  }

  if ( !kernel ){
      std::cout << pro.name() << " :-- Kernel is not valid!\n";
      return false;
  }

  if (ocp_type == "float") {
    bvxm_voxel_grid<float> *grid_out= new bvxm_voxel_grid<float>(out_grid_path, input_grid->grid_size());
    if (auto * float_input_grid=dynamic_cast<bvxm_voxel_grid<float> *>(input_grid.ptr()))
    {
      if (functor_name == "edge2d") {
        bvpl_edge2d_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        return true;
      }
      else if (functor_name == "edge_algebraic_mean") {
        bvpl_edge_algebraic_mean_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge_algebraic_mean_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        return true;
      }
      else if (functor_name == "edge_geometric_mean") {
        bvpl_edge_geometric_mean_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge_geometric_mean_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        return true;
      }
      else {
        std::cout << "Unsupported data type or functor type\n";
        return false;
      }
    }
    std::cout << "Input grid is invalid\n";
    return false;
  }
  else if (ocp_type == "opinion") {
    if (auto * bvxm_opinion_input_grid=dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(input_grid.ptr()))
    {
      bvxm_voxel_grid<bvxm_opinion> * grid_out= new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, bvxm_opinion_input_grid->grid_size());
      grid_out->initialize_data(bvxm_opinion(0.005f));
      bvpl_opinion_functor func;
      bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
      oper.operate(bvxm_opinion_input_grid, kernel, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
      return true;
    }
    std::cout << "Input grid is invalid\n";
    return false;
  }
  else if (ocp_type == "bsta_gauss_f1") {
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    bvxm_voxel_grid<gauss_type> *grid_out= new bvxm_voxel_grid<gauss_type>(out_grid_path, input_grid->grid_size());
    if (auto * gauss_input_grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(input_grid.ptr())){
      if (functor_name == "gauss_convolution") {
        bvpl_gauss_convolution_functor func;
        bvpl_neighb_operator<gauss_type, bvpl_gauss_convolution_functor> oper(func);
        oper.operate(gauss_input_grid, kernel, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        return true;
      }
      if (functor_name == "positive_gauss_convolution") {
        bvpl_positive_gauss_conv_functor func;
        bvpl_neighb_operator<gauss_type, bvpl_positive_gauss_conv_functor> oper(func);
        oper.operate(gauss_input_grid, kernel, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        return true;
      }
      else {
        std::cout << "Unsupported functor\n";
        return false;
      }
    }
    std::cout << "Input grid is invalid\n";
    return false;
  }
  std::cout << "Unsupported data type or functor type\n";
  return false;
}
