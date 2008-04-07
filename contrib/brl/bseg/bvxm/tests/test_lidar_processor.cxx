#include <testlib/testlib_test.h>
#include "bvxm_voxel_slab_base.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_lidar_processor.h"
#include "bvxm_memory_chunk.h"


#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <vnl/vnl_random.h>

namespace
{
  void init_slab(bvxm_voxel_slab<float>& slab)
  {
    unsigned i = 0;
      for (unsigned int y=0; y<slab.ny(); ++y)
        for (unsigned int x=0; x<slab.nx(); ++x, i+=10)
          slab(x,y) = i;
          
  }
};


MAIN( test_lidar_processor )
{
   bvxm_voxel_slab<float> obs(5,6,1);
   init_slab(obs);

   bvxm_lidar_processor processor(10);

   bvxm_voxel_slab<float> prob = processor.prob_density(150, obs);

  SUMMARY();
}
