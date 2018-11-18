#include "breg3d_zero_occupancy_borders_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <bvxm/bvxm_voxel_world.h>


breg3d_zero_occupancy_borders_process::breg3d_zero_occupancy_borders_process()
{
  // process takes 2 inputs:
  //input[0]: The border size
  //input[2]: The voxel world
  input_data_.resize(2,brdb_value_sptr(nullptr));
  input_types_.resize(2);
  input_types_[0] = "unsigned";
  input_types_[1] = "bvxm_voxel_world_sptr";

  // process has 0 outputs.
  output_data_.resize(0,brdb_value_sptr(nullptr));
  output_types_.resize(0);
}


bool breg3d_zero_occupancy_borders_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  auto* input0 =
      static_cast<brdb_value_t<unsigned>* >(input_data_[0].ptr());

  auto* input1 =
      static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[1].ptr());

  // get the plane z level
  unsigned border_size = input0->value();

  // get voxel world
  bvxm_voxel_world_sptr vox_world = input1->value();

  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  bvxm_voxel_grid<ocp_datatype> *grid = dynamic_cast<bvxm_voxel_grid<ocp_datatype>*>(vox_world->get_grid<OCCUPANCY>(0,0).ptr());
  unsigned nz = grid->grid_size().z();
  unsigned nx = grid->grid_size().x();
  unsigned ny = grid->grid_size().y();

  bvxm_voxel_grid<ocp_datatype>::iterator ocp_it = grid->begin();

  ocp_datatype min_prob = vox_world->get_params()->min_occupancy_prob();

  for (unsigned z=0; z < nz; z++, ++ocp_it) {
    bvxm_voxel_slab<ocp_datatype>::iterator vox_it = (*ocp_it).begin();
    unsigned vox_idx = 0;
    for (; vox_it != (*ocp_it).end(); ++vox_it, ++vox_idx) {
      unsigned x = vox_idx % nx;
      unsigned y = vox_idx / nx;
      if ( (x < border_size) || (x >= nx - border_size) || (y < border_size) || (y >= ny - border_size) ) {
        *vox_it = min_prob;
      }
    }
  }
  return true;
}
