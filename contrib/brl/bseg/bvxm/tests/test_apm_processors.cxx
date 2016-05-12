#include <testlib/testlib_test.h>
#include <bvxm/grid/bvxm_voxel_slab_base.h>
#include <bvxm/grid/bvxm_voxel_slab.h>
#include <bvxm/bvxm_mog_grey_processor.h>
#include <bvxm/bvxm_mog_rgb_processor.h>
#include <bvxm/bvxm_mog_mc_processor.h>
#include <bvxm/grid/bvxm_memory_chunk.h>

#include <bsta/bsta_attributes.h>
#include <bsta/bsta_mixture.h>
#include <bsta/bsta_gauss_sf1.h>
#include <bsta/bsta_gauss_if3.h>
#include <bsta/bsta_gaussian_indep.h>
#include <bsta/algo/bsta_adaptive_updater.h>


void test_mog_grey_processor()
{
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_num_obs<bsta_mixture_fixed<gauss_type,3> > mix_gauss_type;
  bvxm_mog_grey_processor processor;
  bool result = true;

  std::cout << "Initializing slabs" << std::endl;
  bvxm_voxel_slab<float> obs(10,10,1);
  obs.fill(0.3f);

  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);

  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());

  // test the  update, expected_color and most_probable_mode_color methods
  std::cout << "Updating model" << std::endl;
  result= result & processor.update(appear, obs, weight);
  obs.fill(0.8f);
  result = result & processor.update(appear, obs, weight);
  obs.fill(0.81f);
  result = result & processor.update(appear, obs, weight);
  TEST("processor.update()", result, true);

  std::cout << "Tetsting statistic methods" << std::endl;

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

  std::cout << "Initializing slabs" << std::endl;
  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> obs(10,10,1);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.3f));

  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);

  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());

  // test the  update, expected_color and most_probable_mode_color methods
  std::cout << "Updating model" << std::endl;
  result = result & processor.update(appear, obs, weight);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.8f));
  result = result & processor.update(appear, obs, weight);
  obs.fill(bvxm_mog_rgb_processor::obs_datatype(0.81f));
  result = result & processor.update(appear, obs, weight);
  TEST("processor.update()", result, true);

  std::cout << "Tetsting statistic methods" << std::endl;

  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> mp_slab  = processor.most_probable_mode_color(appear);
  for (unsigned i=0; i<3; ++i)
  {
    TEST_NEAR("most probable", (*mp_slab.first_voxel())[i], 0.8f,0.01f);
  }

  bvxm_voxel_slab<bvxm_mog_rgb_processor::obs_datatype> mean_slab = processor.expected_color(appear);
  for (unsigned i=0; i<3; ++i)
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

  std::cout << "Initializing slabs" << std::endl;
  bvxm_voxel_slab<data_type_> obs1(10,10,1);
  obs1.fill(data_type_(0.3f));

  bvxm_voxel_slab<float> weight(10,10,1);
  weight.fill(0.01f);

  bvxm_voxel_slab<mix_gauss_type> appear(10,10,1);
  appear.fill(mix_gauss_type());

   // test the  update, expected_color and most_probable_mode_color methods
  std::cout << "Updating model observation 1" << std::endl;
  result = result & processor.update(appear, obs1, weight);
  bvxm_voxel_slab<data_type_> obs2(10,10,1);
  obs2.fill(data_type_(0.8f));
  std::cout << "Updating model observation 2" << std::endl;
  result = result & processor.update(appear, obs2, weight);
  bvxm_voxel_slab<data_type_> obs3(10,10,1);
  obs3.fill(data_type_(0.81f));
  std::cout << "Updating model observation 3" << std::endl;
  result = result & processor.update(appear, obs3, weight);
  TEST("processor.update()", result, true);

  bvxm_voxel_slab<data_type_> mp_slab = processor.most_probable_mode_color(appear);

  for (unsigned i=0; i<dim; i++)
  {
    TEST_NEAR("most probable", (*mp_slab.first_voxel())[i], 0.8f,0.01f);
  }

  bvxm_voxel_slab<data_type_> mean_slab = processor.expected_color(appear);
  for (unsigned i=0; i<dim; i++)
  {
    TEST_NEAR("expected", (*mean_slab.first_voxel())[i],0.63f,0.01f);
  }
}


static void test_apm_processors()
{
  std::cout << "-----------------------------------\n"
           << " Starting mog_grey_processor TESTS\n"
           << "-----------------------------------" <<std::endl;
  test_mog_grey_processor();
  std::cout << "----------------------------------\n"
           << " Starting mog_rgb_processor TESTS\n"
           << "----------------------------------" <<std::endl;
  test_mog_rgb_processor();
  std::cout << "-------------------------------------\n"
           << " Starting mog_mc_2_3_processor TESTS\n"
           << "-------------------------------------" <<std::endl;
  test_mog_mc_processor<2,3>();
  std::cout << "-------------------------------------\n"
           << " Starting mog_mc_3_3_processor TESTS\n"
           << "-------------------------------------" <<std::endl;
  test_mog_mc_processor<3,3>();
  std::cout << "-------------------------------------\n"
           << " Starting mog_mc_4_3_processor TESTS\n"
           << "-------------------------------------" <<std::endl;
  test_mog_mc_processor<4,3>();
}

TESTMAIN(test_apm_processors);
