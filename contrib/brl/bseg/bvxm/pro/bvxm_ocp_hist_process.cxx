#include "bvxm_ocp_hist_process.h"
//#include "bvxm_voxel_world.h"

#include <vil/vil_convert.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>

bvxm_ocp_hist_process::bvxm_ocp_hist_process()
{
  // This process has 2 inputs:
  //input[0]: The voxel world 
  //input[1]: The path for the output file
  input_data_.resize(2, brdb_value_sptr(0));
  input_types_.resize(2);

  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for IMAGE ONLY update
  input_types_[i++] = "vcl_string"; 

  //output
  output_data_.resize(0,brdb_value_sptr(0));
  output_types_.resize(0);
  //int j=0;
  //output_types_[j++]= "double";  // the sum of ocp prob product
}

bool bvxm_ocp_hist_process::execute()
{
  // Sanity check
  if (!this->verify_inputs())
    return false;

  // get the inputs:

  //voxel_world1
  brdb_value_t<bvxm_voxel_world_sptr >* input0 =
    static_cast<brdb_value_t<bvxm_voxel_world_sptr >* >(input_data_[0].ptr());
  bvxm_voxel_world_sptr voxel_world = input0->value();

  //voxel_world2
  brdb_value_t<vcl_string >* input1 =
    static_cast<brdb_value_t<vcl_string >* >(input_data_[1].ptr());
  vcl_string path = input1->value();

  compute(voxel_world, path);

  return true;
}

bool bvxm_ocp_hist_process::compute(bvxm_voxel_world_sptr w,
                                      vcl_string path)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get ocuppancy probability grids
  bvxm_voxel_grid_base_sptr ocp_grid_base = w->get_grid<OCCUPANCY>(0);
  bvxm_voxel_grid<ocp_datatype> *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  vgl_vector_3d<unsigned int> grid_size = w->get_params()->num_voxels();
  bsta_histogram<double> hist(0.0, 1.0, 20, 0.0);
     
  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++ocp_slab_it) {
    bvxm_voxel_slab<float> slab = *ocp_slab_it;
    for (unsigned i=0; i<grid_size.x(); i++) {
      for (unsigned j=0; j<grid_size.y(); j++) {
         hist.upcount(slab(i,j), 1);
      }
    }
  }
  vcl_ofstream f(path.c_str());
  hist.print(f);
  /*vcl_ofstream f2((path+".log").c_str());
  hist.print_log(f2);*/
  return true;
}
