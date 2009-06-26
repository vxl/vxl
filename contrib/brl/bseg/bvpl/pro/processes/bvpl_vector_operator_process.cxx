// This is brl/bseg/bvpl/pro/processes/bvpl_vector_operator_process.cxx

//:
// \file
// \brief A class for applying a vector of kernels on a voxel grid.
//
// \author Isabel Restrepo
// \date June 24, 2009
// \verbatim
//  Modifications
//   
// \endverbatim

#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/bvxm_opinion.h>
#include <bvpl/bvpl_kernel_factory.h>
#include <bvpl/bvpl_edge2d_functor.h>
#include <bvpl/bvpl_opinion_functor.h>
#include <bvpl/bvpl_neighb_operator.h>
#include <bvpl/bvpl_vector_operator.h>
#include <vul/vul_file.h>

namespace bvpl_vector_operator_process_globals
{
  const unsigned n_inputs_ = 6;
  const unsigned n_outputs_ = 2;
}


bool bvpl_vector_operator_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_vector_operator_process_globals;
  //process takes 4inputs
  //input[0]: The grid
  //input[1]: The kernel vector
  //input[2]: The grid type:
  //          -float
  //          -opinion
  //          ....
  //input[4]: The functor type
  //input[5]: Output grid path to hold response
  //input[6]: Output grid path to hold orientations
  vcl_vector<vcl_string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvpl_kernel_vector_sptr";
  input_types_[2] = "vcl_string";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";
  input_types_[5] = "vcl_string";
  if (!pro.set_input_types(input_types_))
    return false;
  
  //output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  return true;
}

bool bvpl_vector_operator_process(bprb_func_process& pro)
{
  using namespace bvpl_vector_operator_process_globals;
  
  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }
  
  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvpl_kernel_vector_sptr kernel = pro.get_input<bvpl_kernel_vector_sptr>(i++);
  vcl_string datatype = pro.get_input<vcl_string>(i++);
  vcl_string functor_name = pro.get_input<vcl_string>(i++);
  vcl_string out_grid_path = pro.get_input<vcl_string>(i++);
  vcl_string orientation_grid_path = pro.get_input<vcl_string>(i++);
  
  //check input's validity
  i=0;
  if (!grid_base.ptr()) {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  
  if ( !kernel ){
    vcl_cout << pro.name() << " :-- Kernel is not valid!\n";
    return false;
  }
  
  if (datatype == "float") {
    bvxm_voxel_grid<float> *grid = dynamic_cast<bvxm_voxel_grid<float>* > (grid_base.ptr());
    bvxm_voxel_grid<float> *grid_out=new bvxm_voxel_grid<float>(out_grid_path, grid->grid_size());
    bvxm_voxel_grid<vnl_vector_fixed<float,3> > *orientation_grid=new bvxm_voxel_grid<vnl_vector_fixed<float,3> >(orientation_grid_path, grid->grid_size());
    if (functor_name == "edge2d") {
      bvpl_edge2d_functor<float> func;
      bvpl_neighb_operator<float, bvpl_edge2d_functor<float> > oper(func);
      bvpl_vector_operator<float, bvpl_edge2d_functor<float> > vector_oper;
      vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, orientation_grid);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
      pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, orientation_grid);
    }
  } else if (datatype == "opinion") {
   bvxm_voxel_grid<bvxm_opinion> *grid = dynamic_cast<bvxm_voxel_grid<bvxm_opinion>* > (grid_base.ptr());;
   bvxm_voxel_grid<bvxm_opinion> *grid_out=new bvxm_voxel_grid<bvxm_opinion>(out_grid_path, grid->grid_size());
   bvxm_voxel_grid<vnl_vector_fixed<float,3> > *orientation_grid=new bvxm_voxel_grid<vnl_vector_fixed<float,3> >(orientation_grid_path, grid->grid_size());
   bvpl_opinion_functor func;
   bvpl_neighb_operator<bvxm_opinion, bvpl_opinion_functor> oper(func);
   bvpl_vector_operator<bvxm_opinion,  bvpl_opinion_functor> vector_oper;
   vector_oper.apply_and_suppress(grid,kernel,&oper,grid_out, orientation_grid);
   pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
   pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, orientation_grid);   
  }
  
  return true;
}



