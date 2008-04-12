#include <testlib/testlib_test.h>
#include "bvxm_voxel_slab_base.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_mog_grey_processor.h"
#include "bvxm_memory_chunk.h"

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_adaptive_updater.h>

#include <vnl/vnl_random.h>

namespace
{
  void init_random_slab(bvxm_voxel_slab<float>& slab)
  {
    vnl_random rand;
      for (unsigned int y=0; y<slab.ny(); ++y)
        for (unsigned int x=0; x<slab.nx(); ++x)
          slab(x,y) = static_cast<float>(rand.drand32());
  }

  void add_random_noise(bvxm_voxel_slab<float>& slab, float std)
  {
    vnl_random rand;
      for (unsigned int y=0; y<slab.ny(); ++y)
        for (unsigned int x=0; x<slab.nx(); ++x){
          slab(x,y) = slab(x,y) + static_cast<float>(rand.normal()*std);
          if (slab(x,y)>1.0f) slab(x,y) = 1.0f;
          if (slab(x,y)<0.0f) slab(x,y) = 0.0f;
        }
  }
};


MAIN( test_apm_processors )
{
   bvxm_voxel_slab<float> obs(10,10,1);
   init_random_slab(obs);

   bvxm_voxel_slab<float> weight(10,10,1);
   init_random_slab(weight);

   typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
   typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;

   bsta_gauss_f1 init_gauss( 0.0f, 0.01f );

   bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);

   appear.fill(mix_gauss_type());

  bvxm_mog_grey_processor processor;

  bool a = processor.update(appear, obs, weight);
  TEST("processor.update()", a, true);

  SUMMARY();
}
