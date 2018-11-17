//This is brl/bseg/bvxm/pro/processes/bvxm_ocp_hist_process.cxx
#include "bvxm_ocp_hist_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>
#include <bprb/bprb_parameters.h>
#include <bsta/bsta_histogram.h>
#include <vgl/vgl_vector_3d.h>
#include <bvxm/bvxm_world_params.h>
#include <bvxm/bvxm_voxel_world.h>

//: set input and output types
bool bvxm_ocp_hist_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_ocp_hist_process_globals;
  // This process has 2 inputs:
  //input[0]: The voxel world
  //input[1]: The path for the output file
  //input[2]: The world scale

  std::vector<std::string> input_types_(n_inputs_);
  int i=0;
  input_types_[i++] = "bvxm_voxel_world_sptr";    // voxel_world for IMAGE ONLY update
  input_types_[i++] = "vcl_string";
  input_types_[i++] = "unsigned";
  return pro.set_input_types(input_types_);
}

//: generates a histogram out of occupancy probability grid
bool bvxm_ocp_hist_process(bprb_func_process& pro)
{
  using namespace bvxm_ocp_hist_process_globals;

  // check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() << " The input number should be " << n_inputs_<< std::endl;
    return false;
  }
  // get the inputs:
  unsigned i = 0;
  // voxel_world
  bvxm_voxel_world_sptr voxel_world = pro.get_input< bvxm_voxel_world_sptr>(i++);
  // the output path
  std::string path = pro.get_input<std::string>(i++);
  // scale
  auto scale = pro.get_input<unsigned>(i++);

  compute(voxel_world,scale, path);

  return true;
}

bool bvxm_ocp_hist_process_globals::compute(const bvxm_voxel_world_sptr& w,
                                            unsigned scale, const std::string& path)
{
  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get occupancy probability grids
  bvxm_voxel_grid_base_sptr ocp_grid_base = w->get_grid<OCCUPANCY>(0, scale);
  auto *ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(ocp_grid_base.ptr());
  bvxm_voxel_grid<ocp_datatype>::const_iterator ocp_slab_it = ocp_grid->begin();

  vgl_vector_3d<unsigned int> grid_size = w->get_params()->num_voxels(scale);
  bsta_histogram<double> hist(0.0, 1.0, 20, 0.0);

  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++ocp_slab_it) {
    bvxm_voxel_slab<float> slab = *ocp_slab_it;
    for (unsigned i=0; i<grid_size.x(); i++) {
      for (unsigned j=0; j<grid_size.y(); j++) {
        hist.upcount(slab(i,j), 1);
      }
    }
  }
  std::ofstream f(path.c_str());
  hist.print(f);
  /*std::ofstream f2((path+".log").c_str());
  hist.print_log(f2);*/
  return true;
}
