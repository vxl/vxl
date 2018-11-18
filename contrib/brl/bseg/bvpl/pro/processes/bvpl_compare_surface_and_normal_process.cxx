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
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 1;
}


bool bvpl_compare_surface_and_normal_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_compare_surface_and_normal_process_globals;

  //process takes 3 inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr"; // distance transform of the gt
  input_types_[1] = "bvxm_voxel_grid_base_sptr"; // estimated response
  input_types_[2] = "vcl_string"; // path for the output grid

  //output
  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_compare_surface_and_normal_process(bprb_func_process& pro)
{
  using namespace bvpl_compare_surface_and_normal_process_globals;

  if (pro.n_inputs() < n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }

  //get inputs:
  unsigned i = 0;
  bvxm_voxel_grid_base_sptr dt_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  bvxm_voxel_grid_base_sptr est_grid_base = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  std::string filename = pro.get_input<std::string>(i++);

  if (!dt_grid_base.ptr() || !est_grid_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  auto *dt_grid = dynamic_cast<bvxm_voxel_grid<float>* > (dt_grid_base.ptr());
  auto *est_grid = dynamic_cast<bvxm_voxel_grid<bvxm_opinion>* > (est_grid_base.ptr());

  bvxm_voxel_grid<float> * gridout=new bvxm_voxel_grid<float>(filename,dt_grid->grid_size());
  bvxm_voxel_grid_compare(dt_grid,est_grid,gridout);

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, gridout);

  return true;
}
