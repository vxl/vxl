// This is brl/bseg/bvpl/pro/processes/bvpl_extract_top_response_process.cxx
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A class visualizing a response from a vector of top responses
//
// \author Isabel Restrepo
// \date September 17, 2009
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

namespace bvpl_extract_top_response_process_globals
{
  constexpr unsigned n_inputs_ = 5;
  constexpr unsigned n_outputs_ = 2;
}

//: process takes 5 inputs and has 2 outputs.
// * input[0]: The response grid (3-D)
// * input[1]: The id grid (3-D)
// * input[2]: The array index (0 corresponds to strongest)
// * input[3]: Path to output response grid
// * input[4]: Path to output id grid
//
// * output[0]: Output response grid(1-D)
// * output[1]: Output id grid (1-D)

bool bvpl_extract_top_response_process_cons(bprb_func_process& pro)
{
  using namespace bvpl_extract_top_response_process_globals;

  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";
  input_types_[4] = "vcl_string";

  std::vector<std::string> output_types_(n_outputs_);
  output_types_[0]= "bvxm_voxel_grid_base_sptr";
  output_types_[1] = "bvxm_voxel_grid_base_sptr";

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

bool bvpl_extract_top_response_process(bprb_func_process& pro)
{
  using namespace bvpl_extract_top_response_process_globals;

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
  auto target_index = pro.get_input<unsigned>(i++);
  std::string out_resp_path = pro.get_input<std::string>(i++);
  std::string out_id_path = pro.get_input<std::string>(i++);

  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;

  //check input's validity
  if (!grid_base.ptr()) {
    std::cout <<  " :-- Grid is not valid!\n";
    return false;
  }

  if (auto *grid = dynamic_cast<bvxm_voxel_grid<vnl_vector_fixed<float,3> >* > (grid_base.ptr())){
    auto *id_grid=dynamic_cast<bvxm_voxel_grid<vnl_vector_fixed<int,3>  >* >(id_grid_base.ptr());
    bvxm_voxel_grid<float> *out_grid = new bvxm_voxel_grid<float>(out_resp_path, id_grid->grid_size());
    bvxm_voxel_grid<int> *out_id_grid = new bvxm_voxel_grid<int>(out_id_path, id_grid->grid_size());
    out_grid-> initialize_data(0.0f);
    out_id_grid->initialize_data(-1);

    bvpl_vector_operator vector_oper;
    vector_oper.get_response(grid, id_grid, target_index, out_grid, out_id_grid);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, out_grid);
    pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, out_id_grid);
    return true;
  }
  else
    return false;
}
