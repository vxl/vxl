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
  typedef bsta_num_obs<bsta_gauss_f1> gauss_type;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;
  bvxm_mog_grey_processor processor;
  bool result = true;

  vcl_cout << "Initializing slabs " << vcl_endl; 
  bvxm_voxel_slab<float> obs(10,10,1);
  obs.fill(0.3f);
  
  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);
  
  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());

  // test the  update, expected_color and most_probable_mode_color methods   
  vcl_cout << "Updating model " << vcl_endl; 
  result= result & processor.update(appear, obs, weight);
  obs.fill(0.8f);
  result = result & processor.update(appear, obs, weight);
  obs.fill(0.81f);
  result = result & processor.update(appear, obs, weight);
  TEST("processor.update()", result, true);

  vcl_cout << "Tetsting statistic methods" << vcl_endl; 

  bvxm_voxel_slab<float> mp_slab = processor.most_probable_mode_color(appear);
  TEST_NEAR("most probable", *(mp_slab.first_voxel()), 0.8f, 0.01f);

  bvxm_voxel_slab<float> mean_slab= processor.expected_color(appear);
  TEST_NEAR("expected", *(mean_slab.first_voxel()), 0.63f, 0.01f);
}

void test_mog_rgb_processor()
{
  typedef bsta_num_obs<bsta_gauss_if3> gauss_type;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;
  bvxm_mog_rgb_processor processor;
  bool result = true;

  vcl_cout << "Initializing slabs " << vcl_endl; 
  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> obs(10,10,1);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.3f));
  
  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);

  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());

  // test the  update, expected_color and most_probable_mode_color methods
  vcl_cout << "Updating model " << vcl_endl; 
  result = result & processor.update(appear, obs, weight);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.8f));
  result = result & processor.update(appear, obs, weight);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.81f));
  result = result & processor.update(appear, obs, weight);
  TEST("processor.update()", result, true);

  vcl_cout << "Tetsting statistic methods" << vcl_endl; 

  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> mp_slab  = processor.most_probable_mode_color(appear);
  for(unsigned i=0; i<3; ++i)
  {
    TEST_NEAR("most probable", (*mp_slab.first_voxel())[i], 0.8f,0.01f);
  }
  
  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> mean_slab = processor.expected_color(appear);
  for(unsigned i=0; i<3; ++i)
  {
  TEST_NEAR("expected", (*mean_slab.first_voxel())[i],0.63f,0.01f);
  }
}

template <unsigned dim, unsigned modes>
void test_mog_mc_processor()
{
  typedef bsta_num_obs<bsta_gaussian_indep<float,dim> > gauss_type;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,modes> > mix_gauss_type;
  typedef typename bvxm_mog_mc_processor<dim,modes>::obs_datatype data_type_;
  bvxm_mog_mc_processor<dim,modes> processor;
  bool result = true;

  vcl_cout << "Initializing slabs " << vcl_endl; 
  bvxm_voxel_slab<data_type_> obs1(10,10,1);
  obs1.fill(data_type_(0.3f));
  
  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);

  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());
  
   // test the  update, expected_color and most_probable_mode_color methods
  vcl_cout << "Updating model observation 1" << vcl_endl; 
  result = result & processor.update(appear, obs1, weight);
  bvxm_voxel_slab<data_type_> obs2(10,10,1);
  obs2.fill(data_type_(0.8f));
  vcl_cout << "Updating model observation 2" << vcl_endl; 
  result = result & processor.update(appear, obs2, weight);
  bvxm_voxel_slab<data_type_> obs3(10,10,1);
  obs3.fill(data_type_(0.81f));
  vcl_cout << "Updating model observation 3" << vcl_endl; 
  result = result & processor.update(appear, obs3, weight);
  TEST("processor.update()", result, true);

  bvxm_voxel_slab<data_type_> mp_slab = processor.most_probable_mode_color(appear);

  for(unsigned i=0; i<dim; i++)
  {
    TEST_NEAR("most probable", (*mp_slab.first_voxel())[i], 0.8f,0.01f);
  }
  
  bvxm_voxel_slab<data_type_> mean_slab = processor.expected_color(appear);
  for(unsigned i=0; i<dim; i++)
  {
    TEST_NEAR("expected", (*mean_slab.first_voxel())[i],0.63f,0.01f);
  }
}


MAIN( test_apm_processors )
{
  vcl_cout << "-----------------------------------\n"
          << " Starting mog_grey_processor TESTS\n"
          << "-----------------------------------" <<vcl_endl;
  test_mog_grey_processor();
  vcl_cout << "----------------------------------\n"
          << " Starting mog_rgb_processor TESTS\n"
          << "----------------------------------" <<vcl_endl;
  test_mog_rgb_processor();
  vcl_cout << "-------------------------------------\n"
          << " Starting mog_mc_2_3_processor TESTS\n"
          << "-------------------------------------" <<vcl_endl;
  test_mog_mc_processor<2,3>();
  vcl_cout << "-------------------------------------\n"
          << " Starting mog_mc_3_3_processor TESTS\n"
          << "-------------------------------------" <<vcl_endl;
  test_mog_mc_processor<3,3>();
  vcl_cout << "-------------------------------------\n"
          << " Starting mog_mc_4_3_processor TESTS\n"
          << "-------------------------------------" <<vcl_endl;
  test_mog_mc_processor<4,3>();
  SUMMARY();
}
