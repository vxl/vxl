#include <testlib/testlib_test.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_lidar_processor.h>
#include <bvxm/grid/bvxm_memory_chunk.h>

#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_adaptive_updater.h>

namespace
{
  void init_slab(bvxm_voxel_slab<float>& slab)
  {
    float i = 0.0f;
      for (unsigned int y=0; y<slab.ny(); ++y)
        for (unsigned int x=0; x<slab.nx(); ++x, i+=10.0f)
          slab(x,y) = i;
  }
};


static void test_lidar_processor()
{
  bvxm_voxel_slab<float> obs(5,6,1);
  init_slab(obs);

  bvxm_lidar_processor processor(10);

  bvxm_voxel_slab<float> prob = processor.prob_density(150, obs);
}

TESTMAIN(test_lidar_processor);
