// This is brl/bseg/bvxm/grid/pro/processes/bvxm_combine_grids_process.cxx
#include <iostream>
#include <string>
#include <bprb/bprb_func_process.h>
//:
// \file
// \brief A process for combining an orientation grid and a response grid
// \author Isabel Restrepo
// \date July 2, 2009
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim

#include <bvxm/grid/bvxm_voxel_grid_basic_ops.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bsta/bsta_attributes.h>
#include <bsta/bsta_gauss_sf1.h>
#include <vnl/vnl_float_3.h>
#include <vnl/vnl_float_4.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: set input and output types
bool bvxm_combine_grids_process_cons(bprb_func_process& pro)
{
  // Inputs
  // 0. grid 1
  // 1. grid2
  // 2. Path to output grid

  std::vector<std::string> input_types_(3);
  input_types_[0] = "bvxm_voxel_grid_base_sptr";
  input_types_[1] = "bvxm_voxel_grid_base_sptr";
  input_types_[2] = "vcl_string";


  // Outputs
  // 0. grid
  std::vector<std::string> output_types_(1);
  output_types_[0] = "bvxm_voxel_grid_base_sptr";
  if (!pro.set_input_types(input_types_))
    return false;

  if (!pro.set_output_types(output_types_))
    return false;

  return true;
}


//: Execute the process
bool bvxm_combine_grids_process(bprb_func_process& pro)
{
  // check number of inputs
  if (pro.n_inputs() != 3)
  {
    std::cout << pro.name() << "The number of inputs should be " << 3<< std::endl;
    return false;
  }

  bvxm_voxel_grid_base_sptr grid1_base = pro.get_input< bvxm_voxel_grid_base_sptr>(0);
  bvxm_voxel_grid_base_sptr grid2_base = pro.get_input< bvxm_voxel_grid_base_sptr>(1);
  std::string output_path = pro.get_input<std::string>(2);

  //get  and create the grids

  auto *grid1 =dynamic_cast<bvxm_voxel_grid<vnl_float_3>* > (grid1_base.ptr());

  if (auto *grid2 =dynamic_cast<bvxm_voxel_grid<float >* > (grid2_base.ptr()))
  {
    if (grid1->grid_size()!=grid2->grid_size()) {
      std::cerr << "Input grids must be of same size\n";
      return false;
    }
    bvxm_voxel_grid<vnl_float_4> *grid_out = new bvxm_voxel_grid<vnl_float_4>(output_path, grid1->grid_size());

    //combine response grid and orientation grid. the result is a vnl_float_4 where the first 3-numbers correspond to orientation
    // and the last number corresponds to kernel response
    bvxm_voxel_grid<float>::iterator grid2_it = grid2->begin();
    bvxm_voxel_grid<vnl_float_3>::iterator grid1_it = grid1->begin();
    bvxm_voxel_grid<vnl_float_4>::iterator grid_out_it = grid_out->begin();
    for (; grid_out_it!=grid_out->end(); ++grid_out_it, ++grid2_it, ++grid1_it)
    {
      bvxm_voxel_slab<float>::iterator slab2_it =(*grid2_it).begin();
      bvxm_voxel_slab<vnl_float_3>::iterator slab1_it= (*grid1_it).begin();
      bvxm_voxel_slab<vnl_float_4>::iterator out_slab_it = (*grid_out_it).begin();

      for (; out_slab_it!=(*grid_out_it).end(); ++out_slab_it, ++slab2_it, ++slab1_it)
      {
        vnl_float_4 this_feature( (*slab1_it)[0], (*slab1_it)[1], (*slab1_it)[2], *slab2_it);
        *out_slab_it = this_feature;
      }
    }

    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  else if (auto *grid2 =dynamic_cast<bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >* > (grid2_base.ptr()))
  {
    if (grid1->grid_size()!=grid2->grid_size()) {
      std::cerr << "Input grids must be of same size\n";
      return false;
    }
    bvxm_voxel_grid<vnl_float_4> *grid_out = new bvxm_voxel_grid<vnl_float_4>(output_path, grid1->grid_size());

    //combine response grid and orientation grid. the result is a vnl_float_4 where the first 3-numbers correspond to orientation
    // and the last number corresponds to kernel response
    bvxm_voxel_grid<bsta_num_obs<bsta_gauss_sf1> >::iterator grid2_it = grid2->begin();
    bvxm_voxel_grid<vnl_float_3>::iterator grid1_it = grid1->begin();
    bvxm_voxel_grid<vnl_float_4>::iterator grid_out_it = grid_out->begin();
    for (; grid_out_it!=grid_out->end(); ++grid_out_it, ++grid2_it, ++grid1_it)
    {
      bvxm_voxel_slab<bsta_num_obs<bsta_gauss_sf1> >::iterator slab2_it =(*grid2_it).begin();
      bvxm_voxel_slab<vnl_float_3>::iterator slab1_it= (*grid1_it).begin();
      bvxm_voxel_slab<vnl_float_4>::iterator out_slab_it = (*grid_out_it).begin();

      for (; out_slab_it!=(*grid_out_it).end(); ++out_slab_it, ++slab2_it, ++slab1_it)
      {
        vnl_float_4 this_feature( (*slab1_it)[0], (*slab1_it)[1], (*slab1_it)[2], std::abs(slab2_it->mean()));
        *out_slab_it = this_feature;
      }
    }

    pro.set_output_val<bvxm_voxel_grid_base_sptr>(0, grid_out);
    return true;
  }
  return false;
}
