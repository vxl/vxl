#include <testlib/testlib_test.h>
#include "bvxm_voxel_slab_base.h"
#include "bvxm_voxel_slab.h"
#include "bvxm_mog_grey_processor.h"
#include "bvxm_mog_rgb_processor.h"
#include "bvxm_mog_mc_processor.h"
#include "bvxm_memory_chunk.h"

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_f1.h>
#include <bsta/bsta_gauss_if3.h>
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

void test_mog_grey_processor()
{
   bvxm_voxel_slab<float> obs(10,10,1);
   init_random_slab(obs);

   bvxm_voxel_slab<float> weight(10,10,1);
   init_random_slab(weight);

   typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
   typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;

   
   bvxm_mog_grey_processor processor;

   bool a = true;
  
   // test the  update, expected_color and most_probable_mode_color methods
   bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
   appear.fill(mix_gauss_type());

   obs.fill(0.3f);
   weight.fill(1.0f/100.0f);
   a = a & processor.update(appear, obs, weight);
   obs.fill(0.8f);
   a = a & processor.update(appear, obs, weight);
   obs.fill(0.81f);
   a = a & processor.update(appear, obs, weight);
   TEST("processor.update()", a, true);

   bvxm_voxel_slab<float> out = processor.most_probable_mode_color(appear);
   TEST_NEAR("most probable", *(out.first_voxel()), 0.8f, 0.01f);

   out = processor.expected_color(appear);
   TEST_NEAR("expected", *(out.first_voxel()), 0.63f, 0.01f);


}

void test_mog_rgb_processor()
{
  
   typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
   typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;

   bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> obs(10,10,1);
   bvxm_voxel_slab<float> weight(10,10,1);
 
  
   bvxm_mog_rgb_processor processor;

   bool a = true;


   // test the  update, expected_color and most_probable_mode_color methods
   bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
   appear.fill(mix_gauss_type());

   obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.3f,0.3f,0.3f));
   weight.fill(1.0f/100.0f);
   a = a & processor.update(appear, obs, weight);
   obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.8f, 0.8f, 0.8f));
   a = a & processor.update(appear, obs, weight);
   obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.81f, 0.81f, 0.81f));
   a = a & processor.update(appear, obs, weight);
   TEST("processor.update()", a, true);

   bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> out = processor.most_probable_mode_color(appear);
  // bvxm_mog_rgb_processor::obs_datatype t= *(out.first_voxel());

   
   TEST_NEAR("most probable", (*out.first_voxel())[1], 0.8f,0.01f);
   TEST_NEAR("most probable", (*out.first_voxel())[2], 0.8f,0.01f);
   TEST_NEAR("most probable", (*out.first_voxel())[3], 0.8f,0.01f);

   out = processor.expected_color(appear);
   TEST_NEAR("expected", (*out.first_voxel())[1],0.63f,0.01f);
   TEST_NEAR("expected", (*out.first_voxel())[2],0.63f,0.01f);
   TEST_NEAR("expected", (*out.first_voxel())[3],0.63f,0.01f);



}

void test_mog_mc_processor()
{
   typedef bsta_num_obs<bsta_gaussian_indep<float,3> > gauss_type;
   typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;

   bvxm_voxel_slab<bvxm_mog_mc_processor<3,3>::obs_datatype> obs(10,10,1);
   bvxm_voxel_slab<float> weight(10,10,1);
   
   bvxm_mog_mc_processor<3,3> processor;

   bool a = true;
    
   // test the  update, expected_color and most_probable_mode_color methods
   bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
   appear.fill(mix_gauss_type());

   obs.fill(bvxm_mog_mc_processor<3,3>::obs_datatype(0.3f,0.3f,0.3f));
   weight.fill(1.0f/100.0f);
   a = a & processor.update(appear, obs, weight);
   obs.fill(bvxm_mog_mc_processor<3,3>::obs_datatype(0.8f, 0.8f, 0.8f));
   a = a & processor.update(appear, obs, weight);
   obs.fill(bvxm_mog_mc_processor<3,3>::obs_datatype(0.81f, 0.81f, 0.81f));
   a = a & processor.update(appear, obs, weight);
   TEST("processor.update()", a, true);

   bvxm_voxel_slab<bvxm_mog_mc_processor<3,3>::obs_datatype> out = processor.most_probable_mode_color(appear);
  
   TEST_NEAR("most probable", (*out.first_voxel())[1], 0.8f,0.01f);
   TEST_NEAR("most probable", (*out.first_voxel())[2], 0.8f,0.01f);
   TEST_NEAR("most probable", (*out.first_voxel())[3], 0.8f,0.01f);

   out = processor.expected_color(appear);
   TEST_NEAR("expected", (*out.first_voxel())[1],0.63f,0.01f);
   TEST_NEAR("expected", (*out.first_voxel())[2],0.63f,0.01f);
   TEST_NEAR("expected", (*out.first_voxel())[3],0.63f,0.01f);
}

MAIN( test_apm_processors )
{
   test_mog_grey_processor();
   test_mog_rgb_processor();
   test_mog_mc_processor();
   
   SUMMARY();
}
