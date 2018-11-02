#include <iostream>
#include <testlib/testlib_test.h>
#include "../bvxm_merge_mog.h"
#include <bvxm/grid/bvxm_voxel_grid.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_merge_mog()
{
  std::cout << "test l2 norm for mixtures of gaussians\n";
  //define some known mixtures
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture_fixed<gauss_type, 3> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

  mix_gauss_type f;
  bsta_gauss_sf1 fc1(0,1);
  f.insert(fc1,0.5f);

  bsta_gauss_sf1 fc2(4,1);
  f.insert(fc2,0.4f);

  bsta_gauss_sf1 fc3(12,3);
  f.insert(fc3,0.1f);

  mix_gauss_type g;
  bsta_gauss_sf1 gc1(0,1.5);
  g.insert(gc1,0.6f);

  bsta_gauss_sf1 gc2(3,1);
  g.insert(gc2,0.35f);

  bsta_gauss_sf1 gc3(15,4);
  g.insert(gc3,0.05f);

  mix_gauss_type h;
  bsta_gauss_sf1 hc1(100,0.5);
  h.insert(hc1,0.8f);

  bsta_gauss_sf1 hc2(95,1);
  h.insert(hc2,0.1f);

  bsta_gauss_sf1 hc3(105,1);
  h.insert(hc3,0.1f);

  mix_gauss_type j;
  bsta_gauss_sf1 jc1(100,0.5);
  j.insert(jc1,0.35f);

  bsta_gauss_sf1 jc2(95,1);
  j.insert(jc2,0.35f);

  bsta_gauss_sf1 jc3(105,1);
  j.insert(jc3,0.3f);

  bsta_gauss_sf1 gauss1;
  bsta_gauss_sf1 gauss2;
  bsta_gauss_sf1 gauss3;
  bsta_gauss_sf1 gauss4;


  bvxm_merge_mog::kl_merge(g,gauss1);
  bvxm_merge_mog::kl_merge(f,gauss2);
  bvxm_merge_mog::kl_merge(h,gauss3);
  bvxm_merge_mog::kl_merge(j,gauss4);

  TEST_NEAR("mean1",gauss1.mean(), 1.8,0.01);
  TEST_NEAR("mean2",gauss2.mean(), 2.8,0.01);
  TEST_NEAR("mean3",gauss3.mean(), 100.0,0.01);
  TEST_NEAR("mean4",gauss4.mean(), 99.75,0.01);

  TEST_NEAR("var1",gauss1.var(), 12.61,0.01);
  TEST_NEAR("var2",gauss2.var(), 14.16,0.01);
  TEST_NEAR("var3",gauss3.var(), 5.6,0.01);
  TEST_NEAR("var4",gauss4.var(), 17.0125,0.01);

  //the sencond part of this test is to test that bvxm_merge_mog::kl_merge_grid work

  //create grids

  vgl_vector_3d<unsigned> grid_size(2,2,2);

  auto *apm_grid= new bvxm_voxel_grid<mix_gauss_type>(grid_size);
  apm_grid->initialize_data(g);

  bvxm_voxel_grid_base_sptr merged_base= new bvxm_voxel_grid<gauss_type>(grid_size);

  bvxm_merge_mog::kl_merge_grid(apm_grid, merged_base);

  auto* merged_grid = static_cast<bvxm_voxel_grid<gauss_type>* >(merged_base.ptr());

  //check that the distances are as expected
  for (bvxm_voxel_grid<gauss_type>::iterator grid_it = merged_grid->begin();
       grid_it != merged_grid->end(); ++grid_it)
  {
    for (auto & slab_it : (*grid_it))
    {
       TEST_NEAR("mean1",slab_it.mean(), 1.8,   0.01);
       TEST_NEAR("var1", slab_it.var(),  12.61, 0.01);
    }
  }
}

TESTMAIN(test_merge_mog);
