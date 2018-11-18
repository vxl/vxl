// This is brl/bseg/bvpl/pro/processes/bvpl_operate_ocp_and_app_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for successively running kernels(of a vector), applying non-max suppression and combining with previous results
//        This class operates on an occupancy grid and an appearance grid simultaneously
// \author Isabel Restrepo
// \date September 17, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/functors/bvpl_find_surface_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/functors/bvpl_positive_gauss_conv_functor.h>
#include <bvpl/functors/bvpl_negative_gauss_conv_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_combined_neighb_operator.h>
#include <bvpl/bvpl_discriminative_non_max_suppression.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

namespace bvpl_operate_ocp_and_app_process_globals
{
  constexpr unsigned n_inputs_ = 7;
  constexpr unsigned n_outputs_ = 2;
}

//: process takes 7 inputs and has 2 outputs.
// * input[0]: The occupancy grid -must be float
// * input[1]: The appearance grid -must be unimodal gaussian
// * input[2]: The kernel vector
// * input[3]: The occupancy functor type
// * input[4]: The appearance functor type
// * input[5]: Output grid path to hold response
// * input[6]: Output grid path to hold ids
//
// * output[0]: Output grid with response
// * output[1]: Output grid with ids
bool bvpl_operate_ocp_and_app_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_operate_ocp_and_app_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "bvpl_kernel_vector_sptr";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";
  input_types_[5] = "vcl_string";
  input_types_[6] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_operate_ocp_and_app_process(bprb_func_process& pro)
{
  using namespace bvpl_operate_ocp_and_app_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr ocp_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvxm_voxel_grid_base_sptr app_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  std::string ocp_functor_name = pro.get_input<std::string>(i++);
  std::string app_functor_name = pro.get_input<std::string>(i++);
  std::string out_grid_path = pro.get_input<std::string>(i++);
  std::string id_grid_path = pro.get_input<std::string>(i++);

  //check input's validity
  if (!ocp_grid_base.ptr() || !app_grid_base.ptr()) {
    std::cout <<  " :-- One of the input grids is not valid!\n";
    return false;
  }

  if ( !kernel ) {
    std::cout << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }

  if (auto *ocp_grid = dynamic_cast<bvxm_voxel_grid<float>* > (ocp_grid_base.ptr()))
  {
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    if (auto* app_grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(app_grid_base.ptr()))
    {
      bvxm_voxel_grid<vnl_vector_fixed<float,3> > *grid_out=new bvxm_voxel_grid<vnl_vector_fixed<float,3> >(out_grid_path, ocp_grid->grid_size());
      bvxm_voxel_grid<vnl_vector_fixed<int,3> > *id_grid=new bvxm_voxel_grid<vnl_vector_fixed<int,3> >(id_grid_path, ocp_grid->grid_size());
      if (app_functor_name == "gauss_convolution") {
        bvpl_gauss_convolution_functor app_func;
        if (ocp_functor_name == "find_surface") {
          bvpl_find_surface_functor ocp_functor;
          bvpl_combined_neighb_operator<bvpl_find_surface_functor, bvpl_gauss_convolution_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
        if (ocp_functor_name == "edge_geometric_mean") {
          bvpl_edge_geometric_mean_functor<float>  ocp_functor;
          bvpl_combined_neighb_operator<bvpl_edge_geometric_mean_functor<float>, bvpl_gauss_convolution_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
      }
      if (app_functor_name == "positive_gauss_convolution") {
        bvpl_positive_gauss_conv_functor app_func;
        if (ocp_functor_name == "find_surface") {
          bvpl_find_surface_functor ocp_functor;
          bvpl_combined_neighb_operator<bvpl_find_surface_functor, bvpl_positive_gauss_conv_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
        if (ocp_functor_name == "edge_geometric_mean") {
          bvpl_edge_geometric_mean_functor<float>  ocp_functor;
          bvpl_combined_neighb_operator<bvpl_edge_geometric_mean_functor<float>, bvpl_positive_gauss_conv_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
      }
      if (app_functor_name == "negative_gauss_convolution") {
        bvpl_negative_gauss_conv_functor app_func;
        if (ocp_functor_name == "find_surface") {
          bvpl_find_surface_functor ocp_functor;
          bvpl_combined_neighb_operator<bvpl_find_surface_functor, bvpl_negative_gauss_conv_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
        if (ocp_functor_name == "edge_geometric_mean") {
          bvpl_edge_geometric_mean_functor<float>  ocp_functor;
          bvpl_combined_neighb_operator<bvpl_edge_geometric_mean_functor<float>, bvpl_negative_gauss_conv_functor> oper(ocp_functor,app_func);
          bvpl_discriminative_non_max_suppression vector_oper;
          vector_oper.suppress_and_combine(ocp_grid, app_grid, kernel, &oper, grid_out, id_grid);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
          pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
          return true;
        }
      }
    }
  }

    std::cerr << "Grid type or functor type not supported\n";

   return false;
}
