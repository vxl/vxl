// This is brl/bseg/bvpl/pro/processes/bvpl_vector_operator_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for applying a vector of kernels on a voxel grid.
//
// \author Isabel Restrepo
// \date June 24, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/functors/bvpl_edge2d_functor.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_gauss_convolution_functor.h>
#include <bvpl/functors/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/bvpl_vector_operator.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

namespace bvpl_vector_operator_process_globals
{
  constexpr unsigned n_inputs_ = 6;
  constexpr unsigned n_outputs_ = 2;
}

//: process takes 6 inputs and has 2 outputs.
// input[0]: The grid
// input[1]: The kernel vector
// input[2]: The grid type:
//           - float
//           - opinion
//           - ...
// input[3]: The functor type
// input[4]: Output grid path to hold response
// input[5]: Output grid path to hold ids
//
// output[0]: Output grid with response
// output[1]: Output grid with ids
bool bvpl_vector_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_vector_operator_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvpl_kernel_vector_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";
  input_types_[5] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_vector_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_vector_operator_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << ": the input number should be " << n_inputs_
             << " but instead it is " << pro.n_inputs() << std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  std::string datatype = pro.get_input<std::string>(i++);
  std::string functor_name = pro.get_input<std::string>(i++);
  std::string out_grid_path = pro.get_input<std::string>(i++);
  std::string id_grid_path = pro.get_input<std::string>(i++);

  //check input's validity
  if (!grid_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  if ( !kernel ) {
    std::cout << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }

  if (datatype == "float")
  {
    bvxm_voxel_grid<float> *grid = dynamic_cast<bvxm_voxel_grid<float>* > (grid_base.ptr());
    bvxm_voxel_grid<float> *grid_out=new bvxm_voxel_grid<float>(out_grid_path, grid->grid_size());
    bvxm_voxel_grid<int > *id_grid=new bvxm_voxel_grid<int >(id_grid_path, grid->grid_size());
    if (functor_name == "edge2d") {
      bvpl_edge2d_functor<float> func;
      bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
      bvpl_vector_operator vector_oper;
      vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, id_grid);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
      return true;
    }
    if (functor_name == "edge_algebraic_mean") {
      bvpl_edge_algebraic_mean_functor<float> func;
      bvpl_neighb_operator<float, bvpl_edge_algebraic_mean_functor<float> > oper(func);
      bvpl_vector_operator vector_oper;
      vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, id_grid);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
      return true;
    }
    if (functor_name == "edge_geometric_mean") {
      bvpl_edge_geometric_mean_functor<float> func;
      bvpl_neighb_operator<float, bvpl_edge_geometric_mean_functor<float> > oper(func);
      bvpl_vector_operator vector_oper;
      vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, id_grid);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
      return true;
    }
  }
  else if (datatype == "opinion") {
    bvxm_voxel_grid<bvxm_opinion> *grid = dynamic_cast<bvxm_voxel_grid<bvxm_opinion>* > (grid_base.ptr());;
    bvxm_voxel_grid<bvxm_opinion> *grid_out=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, grid->grid_size());
    bvxm_voxel_grid<int > *id_grid=new bvxm_voxel_grid<int >(id_grid_path, grid->grid_size());
    bvpl_opinion_functor func;
    bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
    bvpl_vector_operator vector_oper;
    vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, id_grid);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
    return true;
  }
  else if (datatype == "bsta_gauss_f1") {
    typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
    if (bvxm_voxel_grid<gauss_type>* grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(grid_base.ptr())) {
      bvxm_voxel_grid<gauss_type> *grid_out= new bvxm_voxel_grid<gauss_type>(out_grid_path, grid->grid_size());
      bvxm_voxel_grid<int > *id_grid=new bvxm_voxel_grid<int >(id_grid_path, grid->grid_size());
      if (functor_name == "gauss_convolution") {
        bvpl_gauss_convolution_functor func;
        bvpl_neighb_operator<gauss_type, bvpl_gauss_convolution_functor> oper(func);
        bvpl_vector_operator vector_oper;
        vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, id_grid);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
        pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, id_grid);
      }
      return true;
    }
    else
      return false;
  }
  return false;
}

