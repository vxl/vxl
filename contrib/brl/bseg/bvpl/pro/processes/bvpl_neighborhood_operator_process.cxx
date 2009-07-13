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
//  <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvpl/bvpl_kernel_factory.h>
#include <bvpl/bvpl_edge2d_functor.h>
#include <bvpl/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>

namespace bvpl_neighborhood_operator_process_globals
{
  const unsigned n_inputs_ = 5;
  const unsigned n_outputs_ = 1;
}


bool bvpl_neighborhood_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_neighborhood_operator_process_globals;
  //process takes 4inputs
  //input[0]: The grid
  //input[1]: The kernel
  //input[2]: The occupancy type:
  //          -float
  //          -opinion
  //input[4]: The functor type
  //input[5]: Output grid path
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvpl_kernel_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;

  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  //output has no output
  return true;
}

bool bvpl_neighborhood_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_neighborhood_operator_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr input_grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_sptr kernel = pro.get_input<bvpl_kernel_sptr>(i++);
  vcl_string ocp_type = pro.get_input<vcl_string>(i++);
  vcl_string functor_name = pro.get_input<vcl_string>(i++);
  vcl_string out_grid_path = pro.get_input<vcl_string>(i++);

  //check input's validity
  i=0;
  if (!input_grid.ptr()) {
    vcl_cout << pro.name() << " :-- Grid is not valid!\n";
    return false;
  }

  if ( !kernel ){
      vcl_cout << pro.name() << " :-- Kernel is not valid!\n";
      return false;
  }

  if (ocp_type == "float") {
    bvxm_voxel_grid<float> *grid_out= new bvxm_voxel_grid<float>(out_grid_path, input_grid->grid_size());
    if (bvxm_voxel_grid<float> * float_input_grid=dynamic_cast<bvxm_voxel_grid<float> *>(input_grid.ptr()))
    {
      if (functor_name == "edge2d") {
        bvpl_edge2d_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
      }
      if (functor_name == "edge_algebraic_mean") {
        bvpl_edge_algebraic_mean_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge_algebraic_mean_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
      }
      if (functor_name == "edge_geometric_mean") {
        bvpl_edge_geometric_mean_functor<float> func;
        bvpl_neighb_operator<float, bvpl_edge_geometric_mean_functor<float> > oper(func);
        oper.operate(float_input_grid, kernel, grid_out);
      }
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    }
  }
  else if (ocp_type == "opinion") {
    if (bvxm_voxel_grid<bvxm_opinion> * bvxm_opinion_input_grid=dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(input_grid.ptr()))
    {
      bvxm_voxel_grid<bvxm_opinion> * grid_out= new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, bvxm_opinion_input_grid->grid_size());
      bvpl_opinion_functor func;
      bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
      oper.operate(bvxm_opinion_input_grid, kernel, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    }
  }

  return true;
}
