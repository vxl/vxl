// This is brl/bseg/bvpl/pro/processes/bvpl_compare_surface_and_normal_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class for comparing the ground truth with the estimated surface and normal
//
// \author Vishal Jain
// \date Aug  11, 2009
// \verbatim
//  Modifications
//
// \endverbatim

#include <bprb/bprb_parameters.h>

#include <brdb/brdb_value.h>

#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>

namespace bvpl_compare_surface_and_normal_process_globals
{
  const unsigned n_inputs_ = 3;
  const unsigned n_outputs_ = 1;
}


bool bvpl_compare_surface_and_normal_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compare_surface_and_normal_process_globals;
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
  input_types_[0] = "bvxm_voxel_grid_base_sptr"; // distance transform of the gt
  input_types_[1] = "bvxm_voxel_grid_base_sptr"; // estimated response
  input_types_[2] = "vcl_string"; // path for the ouput grid

  if (!pro.set_input_types(input_types_))
    return false;

  //output
  vcl_vector<vcl_string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  if (!pro.set_output_types(output_types_))
    return false;
  return true;
}

bool bvpl_compare_surface_and_normal_process(bprb_func_process& pro)
{
  using namespace bvpl_compare_surface_and_normal_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    vcl_cout << pro.name() << " The input number should be " << n_inputs_<< vcl_endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr dt_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvxm_voxel_grid_base_sptr est_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  vcl_string filename = pro.get_input<vcl_string>(i++);

  if (!dt_grid_base.ptr() || !est_grid_base.ptr()) {
    vcl_cout <<  " :-- Grid is not valid!\n";
    return false;
  }
  
  bvxm_voxel_grid<float> *dt_grid = dynamic_cast<bvxm_voxel_grid<float>* > (dt_grid_base.ptr());
  bvxm_voxel_grid<bvxm_opinion> *est_grid = dynamic_cast<bvxm_voxel_grid<bvxm_opinion>* > (est_grid_base.ptr());

  bvxm_voxel_grid<float> * gridout=new bvxm_voxel_grid<float>(filename,dt_grid->grid_size());
  bvxm_voxel_grid_compare(dt_grid,est_grid,gridout);

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, gridout);

  return true;
}

