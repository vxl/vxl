//This is brl/bseg/bvxm/pro/processes/bvxm_compare_3d_voxels_process.cxx
#include <iostream>
#include <fstream>
#include "bvxm_compare_3d_voxels_process.h"
//:
// \file
#include <bprb/bprb_func_process.h>

#include <brip/brip_vil_float_ops.h>

#include <bvxm/bvxm_voxel_world.h>

#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_image_metadata.h>
#include <bvxm/bvxm_voxel_traits.h>
#include <bvxm/bvxm_util.h>
#include <bsta/bsta_histogram.h>

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_compare_3d_voxels_process_cons(bprb_func_process& pro)
{
  using namespace bvxm_compare_3d_voxels_process_globals;
  //inputs
  std::vector<std::string> input_types_(n_inputs_);
  input_types_[0] = "bvxm_voxel_world_sptr";   // world
  input_types_[1] = "bvxm_voxel_world_sptr";
  input_types_[2] = "unsigned";
  input_types_[3] = "vcl_string";

  return pro.set_input_types(input_types_);
}

//: compares voxel worlds
bool bvxm_compare_3d_voxels_process(bprb_func_process& pro)
{
  using namespace bvxm_compare_3d_voxels_process_globals;

  //check number of inputs
  if (pro.n_inputs()<n_inputs_)
  {
    std::cout << pro.name() <<" : The input number should be "<< n_inputs_ << std::endl;
    return false;
  }

  //get the inputs
  unsigned i =0;
  bvxm_voxel_world_sptr main_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  bvxm_voxel_world_sptr multi_scale_world = pro.get_input<bvxm_voxel_world_sptr>(i++);
  auto input_scale = pro.get_input<unsigned>(i++);
  std::string filename = pro.get_input<std::string>(i++);

  typedef bvxm_voxel_traits<OCCUPANCY>::voxel_datatype ocp_datatype;

  // get occupancy probability grids
  bvxm_voxel_grid_base_sptr gt_ocp_grid_base = main_world->get_grid<OCCUPANCY>(0,0);
  auto *gt_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(gt_ocp_grid_base.ptr());
  //bvxm_voxel_grid<ocp_datatype>::const_iterator l_ocp_slab_it = lidar_ocp_grid->begin();

  bvxm_voxel_grid_base_sptr multi_ocp_grid_base = multi_scale_world->get_grid<OCCUPANCY>(0,input_scale);
  auto *multi_ocp_grid  = static_cast<bvxm_voxel_grid<ocp_datatype>*>(multi_ocp_grid_base.ptr());

  vgl_vector_3d<unsigned int> grid_size = multi_scale_world->get_params()->num_voxels(input_scale);
  bsta_histogram<double> hist(0.0, 1.0, 20, 0.0);
  bvxm_voxel_grid<ocp_datatype>::const_iterator m_ocp_slab_it = multi_ocp_grid->begin();

  unsigned int factor = 1<<input_scale;
  for (unsigned k_idx = 0; k_idx < (unsigned)grid_size.z(); ++k_idx, ++m_ocp_slab_it) {
    bvxm_voxel_slab<float> m_slab = *m_ocp_slab_it;

    vgl_vector_3d<unsigned int> gt_grid_size = main_world->get_params()->num_voxels();

    int gt_k_idx=k_idx*factor;
    bvxm_voxel_grid<ocp_datatype>::const_iterator gt_ocp_slab_it = gt_ocp_grid->slab_iterator(gt_k_idx);
    bvxm_voxel_slab<float> gt_slab = *gt_ocp_slab_it;

    for (unsigned i=0; i<grid_size.x(); i++) {
      for (unsigned j=0; j<grid_size.y(); j++) {
        double min_err=2;

        for (unsigned int gt_i=i*factor-1;gt_i<=i*factor+1;++gt_i)
        {
          for (unsigned int gt_j=j*factor-1;gt_j<=j*factor+1;++gt_j)
          {
            if (gt_i<gt_grid_size.x() && gt_j<gt_grid_size.y())
            {
              if (m_slab(i,j)>multi_scale_world->get_params()->min_occupancy_prob() &&
                  gt_slab(gt_i,gt_j) >main_world->get_params()->min_occupancy_prob())
              {
                double err=2*std::fabs(m_slab(i,j)-gt_slab(gt_i,gt_j));
                err/=(m_slab(i,j)+gt_slab(gt_i,gt_j));
                if (err<min_err)
                  min_err=err;
              }
            }
          }
        }
        if (min_err<=1)
          hist.upcount(min_err, 1);
      }
    }
  }

  std::ofstream ofile(filename.c_str());
  if (!ofile)
  {
    std::cerr<<"Couldn't open file "<<filename<< '\n';
    return false;
  }

  hist.print(ofile);

  ofile.close();

  return true;
}
