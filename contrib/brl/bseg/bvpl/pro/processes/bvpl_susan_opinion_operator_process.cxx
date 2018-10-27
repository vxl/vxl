// This is brl/bseg/bvpl/pro/processes/bvpl_susan_opinion_operator_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for applying a susan kernel on a normals of the voxel world.
//
// \author Vishal Jain
// \date Aug 13, 2009
// \verbatim
//  Modifications
//  <none yet>
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvpl/kernels/bvpl_kernel_factory.h>
#include <bvpl/functors/bvpl_edge2d_functor.h>
#include <bvpl/functors/bvpl_edge_geometric_mean_functor.h>
#include <bvpl/functors/bvpl_edge_algebraic_mean_functor.h>
#include <bvpl/functors/bvpl_opinion_functor.h>
#include <bvpl/bvpl_susan_opinion_operator.h>

namespace bvpl_susan_opinion_operator_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_susan_opinion_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_susan_opinion_operator_process_globals;
  //process takes 4inputs
  //input[0]: The grid
  //input[1]: The kernel
  //input[2]: The occupancy type:
  //          -float
  //          -opinion
  //input[4]: The functor type
  //input[5]: Output grid path
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr"; //: normals grid
  input_types_[1] = "bvxm_voxel_grid_base_sptr"; //: response grid
  input_types_[2] = "vcl_string";                //: path for the output grid
  input_types_[3] = "bvpl_kernel_vector_sptr";          //: pointer to a kernel
  if (!pro.set_input_types(input_types_))
    return false;

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;

  //output has no output
  return true;
}

bool bvpl_susan_opinion_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_susan_opinion_operator_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr in_dir_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvxm_voxel_grid_base_sptr in_opn_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  std::string out_grid_path = pro.get_input<std::string>(i++);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(i++);


  auto * in_dir_grid
      =dynamic_cast<bvxm_voxel_grid<int> *>(in_dir_grid_base.ptr());
  auto * in_opn_grid
      =dynamic_cast<bvxm_voxel_grid<bvxm_opinion> *>(in_opn_grid_base.ptr());

  if(in_opn_grid)
    std::cout<<"Successful cast"<<std::endl;
  bvxm_voxel_grid<bvxm_opinion> * output_grid=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path,in_dir_grid->grid_size());
  bvpl_susan_opinion_operator oper;
  oper.operate(in_dir_grid,in_opn_grid,kernel,output_grid);

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, output_grid);

  return true;
}
