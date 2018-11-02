#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include <vgl/vgl_vector_3d.h>

#include "../bvxm_voxel_grid_basic_ops.h"


void multiply()
{
  vgl_vector_3d<unsigned> grid_size(2,2,2);
  bvxm_voxel_grid_base_sptr grid1_base = new  bvxm_voxel_grid<float>(grid_size);
  bvxm_voxel_grid_base_sptr grid2_base = new bvxm_voxel_grid<float>(grid_size);
  bvxm_voxel_grid_base_sptr grid_out_base = new bvxm_voxel_grid<float>(grid_size);

  auto* grid1 = dynamic_cast<bvxm_voxel_grid<float>* >(grid1_base.ptr());
  auto* grid2 = dynamic_cast<bvxm_voxel_grid<float>* >(grid2_base.ptr());
  auto* grid_out = dynamic_cast<bvxm_voxel_grid<float>* >(grid_out_base.ptr());

  grid1->initialize_data(2.0f);
  grid2->initialize_data(4.0f);
  grid_out->initialize_data(0.0f);


  bvxm_voxel_grid_multiply<float> (grid1_base, grid2_base, grid_out_base);

  bvxm_voxel_grid<float>::iterator grid_it = grid_out->begin();
  for (; grid_it!=grid_out->end(); ++grid_it)
  {
    bvxm_voxel_slab<float>::iterator slab_it = grid_it->begin();
    for (; slab_it!=grid_it->end(); ++slab_it)
    {
      float val = (*slab_it);
      TEST_NEAR("Val",val, 8.0f ,0.001f);
    }
  }
}

void threshold()
{
  vgl_vector_3d<unsigned> grid_size(2,2,2);
  bvxm_voxel_grid_base_sptr grid_base = new  bvxm_voxel_grid<float>(grid_size);
  bvxm_voxel_grid_base_sptr mask_base = new bvxm_voxel_grid<bool>(grid_size);
  bvxm_voxel_grid_base_sptr grid_out_base = new bvxm_voxel_grid<float>(grid_size);

  auto* grid = dynamic_cast<bvxm_voxel_grid<float>* >(grid_base.ptr());
  auto* mask = dynamic_cast<bvxm_voxel_grid<bool>* >(mask_base.ptr());
  auto* grid_out = dynamic_cast<bvxm_voxel_grid<float>* >(grid_out_base.ptr());

  grid->initialize_data(5.0f);
  mask->initialize_data(false);
  grid_out->initialize_data(0.0f);


  bvxm_voxel_grid_threshold<float> (grid_base, grid_out_base, mask_base ,2.0f);

  bvxm_voxel_grid<float>::iterator grid_it = grid_out->begin();
  for (; grid_it!=grid_out->end(); ++grid_it)
  {
    bvxm_voxel_slab<float>::iterator slab_it = grid_it->begin();
    for (; slab_it!=grid_it->end(); ++slab_it)
    {
      bool result = (*slab_it) > 2.0f;
      TEST("Threshold",result, true);
    }
  }
}

static void test_basic_ops()
{
  multiply();
  threshold();
}

TESTMAIN(test_basic_ops);
