#include "breg3d_set_occupancy_plane_process.h"

#include <brdb/brdb_value.h>
#include <bprb/bprb_parameters.h>

#include <vgl/vgl_point_3d.h>
#include <vnl/vnl_math.h>

#include <bvxm/bvxm_voxel_world.h>


breg3d_set_occupancy_plane_process::breg3d_set_occupancy_plane_process()
{
  // process takes 3 inputs: 
  //inputs[0-3]: The plane parameters (a,b,c,d) such that ax + by + cz + d = 0
  //input[4]: The standard deviation of the plane estimate
  //input[5]: The voxel world
  input_data_.resize(6,brdb_value_sptr(0));
  input_types_.resize(6);
  input_types_[0] = "double";
  input_types_[1] = "double";
  input_types_[2] = "double";
  input_types_[3] = "double";
  input_types_[4] = "double";
  input_types_[5] = "bvxm_voxel_world_sptr";

  // process has 0 outputs.
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);

}


bool breg3d_set_occupancy_plane_process::execute()
{

  // Sanity check
  if(!this->verify_inputs())
    return false;

  brdb_value_t<double>* input0 = 
    static_cast<brdb_value_t<double>* >(input_data_[0].ptr());

  brdb_value_t<double>* input1 = 
    static_cast<brdb_value_t<double>* >(input_data_[1].ptr());

  brdb_value_t<double>* input2 = 
    static_cast<brdb_value_t<double>* >(input_data_[2].ptr());

  brdb_value_t<double>* input3 = 
    static_cast<brdb_value_t<double>* >(input_data_[3].ptr());

  brdb_value_t<double>* input4 = 
    static_cast<brdb_value_t<double>* >(input_data_[4].ptr());

  brdb_value_t<bvxm_voxel_world_sptr>* input5 = 
    static_cast<brdb_value_t<bvxm_voxel_world_sptr>* >(input_data_[5].ptr());


  // get the plane parameters
  double plane_a = input0->value();
  double plane_b = input1->value();
  double plane_c = input2->value();
  double plane_d = input3->value();

  // get the standard deviation
  double plane_std = input4->value();

  // get voxel world
  bvxm_voxel_world_sptr vox_world = input5->value();

  // normalize plane parameters
  double norm_val = vcl_sqrt(plane_a*plane_a + plane_b*plane_b + plane_c*plane_c);
  plane_a /= norm_val;
  plane_b /= norm_val;
  plane_c /= norm_val;
  plane_d /= norm_val;

  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  bvxm_voxel_grid<ocp_datatype> *grid = dynamic_cast<bvxm_voxel_grid<ocp_datatype>*>(vox_world->get_grid<OCCUPANCY>(0,0).ptr());
  unsigned nz = grid->grid_size().z();
  unsigned nx = grid->grid_size().x();
  unsigned ny = grid->grid_size().y();

  ocp_datatype min_prob = vox_world->get_params()->min_occupancy_prob();
  ocp_datatype max_prob = vox_world->get_params()->max_occupancy_prob();

  bvxm_voxel_grid<ocp_datatype>::iterator ocp_it = grid->begin();
  for (unsigned k=0; k < nz; ++k, ++ocp_it) {

    bvxm_voxel_slab<ocp_datatype> slab = *(ocp_it);
    for (unsigned i=0; i < nx; ++i) {
      for (unsigned j=0; j< ny; ++j) {

        vgl_point_3d<float> world_pt = vox_world->voxel_index_to_xyz(i,j,k);

        double dist = world_pt.x() * plane_a + world_pt.y() * plane_b + world_pt.z() * plane_c + plane_d;

        ocp_datatype vox_prob = 
          (ocp_datatype)( vnl_math::sqrt1_2 * vnl_math::two_over_sqrtpi * (0.5/plane_std) * vcl_exp(-(dist*dist)/(2*plane_std*plane_std)) );
        if (vox_prob < min_prob)
          vox_prob = min_prob;
        if (vox_prob > max_prob)
          vox_prob = max_prob;
        slab(i,j) = vox_prob;

      }
    }
  }

  return true;
}


