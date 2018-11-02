// This is brl/bseg/bvxm/grid/pro/processes/bvxm_grid_distance_trasform_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief  A process for computing the distance transform and the distance vectors on a grid.
//         The surface is indicated by voxel with value 0, the rest is a max value.
//         The direction grid contains the direction vectors to the closest surface voxel.
// \author Gamze D. Tunali
// \date   July 29, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bprb/bprb_parameters.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>

namespace bvxm_grid_distance_transform_process_globals
{
  constexpr unsigned n_inputs_ = 3;
  constexpr unsigned n_outputs_ = 2;
}


//: set input and output types
bool bvxm_grid_distance_transform_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_grid_distance_transform_process_globals;

  // process takes 3 inputs and has 2 outputs.
  std::vector<std::string> input_types_(n_inputs_);
  unsigned i=0;
  input_types_[i++]="bvxm_voxel_grid_base_sptr"; // the grid, it will be updtaed with distance values
  input_types_[i++]="vcl_string";                // voxel storage path for direction grid
  input_types_[i++]="vcl_string";                // voxel storage path for direction grid

  std::vector<std::string> output_types_(n_outputs_);
  i=0;
  output_types_[i++]="bvxm_voxel_grid_base_sptr";  // The resulting direction grid
  output_types_[i++]="bvxm_voxel_grid_base_sptr";  // The resulting distance transform as magnitude of direction grid

  return pro.set_input_types(input_types_) && pro.set_output_types(output_types_);
}

//: Execute the process
bool bvxm_grid_distance_transform_process(bprb_func_process& pro)
{
  using namespace bvxm_grid_distance_transform_process_globals;

  // check number of inputs
  if (pro.input_types().size() != n_inputs_)
  {
    std::cout << pro.name() << "--The number of inputs should be " << n_inputs_ << std::endl;
    return false;
  }

  unsigned i=0;
  bvxm_voxel_grid_base_sptr grid = pro.get_input<bvxm_voxel_grid_base_sptr>(i++);
  std::string vox_path = pro.get_input<std::string>(i++);
  std::string mag_path = pro.get_input<std::string>(i++);


  if (!grid) {
    std::cout << pro.name() << "--The input grid is not valid " << std::endl;
    return false;
  }

  bvxm_voxel_grid<vnl_vector_fixed<float,3> >* dir = new bvxm_voxel_grid<vnl_vector_fixed<float,3> >(vox_path,grid->grid_size());
  bvxm_voxel_grid<float>* mag = new bvxm_voxel_grid<float>(mag_path,grid->grid_size());

  dir->initialize_data(vnl_vector_fixed<float,3>(0,0,0));
  auto* g = static_cast<bvxm_voxel_grid<float>*>(grid.as_pointer());
  bvxm_grid_dist_transform<float>(g,dir,mag);

  pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, dir);
  pro.set_output_val<bvxm_voxel_grid_base_sptr>(1, mag);

  return true;
}
