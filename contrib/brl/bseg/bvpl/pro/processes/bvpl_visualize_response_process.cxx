// This is brl/bseg/bvpl/pro/processes/bvpl_visualize_response_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class visualizing the response of a particular id
//
// \author Isabel Restrepo
// \date August 27, 2009
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim


#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_opinion.h>
#include <bvxm/grid/io/bvxm_io_voxel_grid.h>
#include <bvpl/kernels/bvpl_kernel.h>
#include <bvpl/bvpl_vector_operator.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>

#include <vpl/vpl.h>

namespace bvpl_visualize_response_process_globals
{
  constexpr unsigned n_inputs_ = 4;
  constexpr unsigned n_outputs_ = 0;
}

//: process takes 6 inputs and has 2 outputs.
// input[0]: The response grid
// input[1]: The id grid
// input[2]: The target id
// input[3]: Raw filename


bool bvpl_visualize_response_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_visualize_response_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  std::vector<std::string> output_types_(n_outputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_visualize_response_process(bprb_func_process& pro)
{
  using namespace bvpl_visualize_response_process_globals;

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
  auto target_id = pro.get_input<unsigned>(i++);
  std::string raw_path = pro.get_input<std::string>(i++);

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

  //check input's validity
  if (!grid_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }


  auto *id_grid=dynamic_cast<bvxm_voxel_grid<int >* >(id_grid_base.ptr());
  bvxm_voxel_grid<float> *temp_grid=new bvxm_voxel_grid<float>("filter_temp.vox", id_grid->grid_size());

  if (auto *grid = dynamic_cast<bvxm_voxel_grid<float>* > (grid_base.ptr())) {
    bvpl_vector_operator vector_oper;
    vector_oper.filter_response(grid, id_grid, target_id, temp_grid);
  }
  else if (auto* grid=dynamic_cast<bvxm_voxel_grid<gauss_type> *>(grid_base.ptr())) {
    bvpl_vector_operator vector_oper;
    vector_oper.filter_response(grid, id_grid, target_id, temp_grid);
  }
  else
    return false;

  bvxm_grid_save_raw<float>(temp_grid,raw_path);
  //clean temporary files
  vpl_unlink("filter_temp.vox");

  return true;
}
