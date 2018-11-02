#include <iostream>
#include <testlib/testlib_test.h>
#include "../bvxm_mog_norm.h"
#include <bvxm/grid/bvxm_voxel_grid.h>
#include <bvxm/grid/bvxm_voxel_grid_base.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

void test_l2_mog_norm()
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


  double dist1 =bvxm_mog_norm<float>::mog_l2(g,f);
  double dist2 = bvxm_mog_norm<float>::mog_l2(h,j);

  TEST_NEAR("G-F",dist1, 0.1602,0.01);
  TEST_NEAR("H-J",dist2, 0.3312,0.01);

  //The sencond part of this test is to test that bseg3d_explore_mixtures::calculate_l2distance work

  //Create grids
  vgl_vector_3d<unsigned> grid_size(2,2,2);

  auto *apm_grid= new bvxm_voxel_grid<mix_gauss_type>(grid_size);
  apm_grid->initialize_data(g);

  auto *mask_grid = new bvxm_voxel_grid<bool>(grid_size);
  mask_grid->initialize_data(true);

  bvxm_voxel_grid_base_sptr dist_base  = new bvxm_voxel_grid<float>(grid_size);

  bvxm_mog_norm<float>::mog_l2_grid(apm_grid,mask_grid,dist_base,true,f);

  auto* dist_grid = static_cast<bvxm_voxel_grid< float>* >(dist_base.ptr());
  //check that the distances are as expected
  bvxm_voxel_grid<float>::iterator dist_grid_it = dist_grid->begin();

  for (; dist_grid_it != dist_grid->end(); ++dist_grid_it)
  {
    bvxm_voxel_slab<float>::iterator dist_slab_it = (*dist_grid_it).begin();
    for (; dist_slab_it != (*dist_grid_it).end(); ++dist_slab_it)
    {
      float dist = (*dist_slab_it);
      TEST_NEAR("Grid",dist, 0.1602,0.01);
    }
  }
}


void test_gauss2mix()
{
  std::cout << "test l2 norm from a gaussian to a mixture\n";
  //define some known mixtures
  typedef bsta_num_obs<bsta_gauss_sf1> gauss_type;
  typedef bsta_mixture<gauss_type> mix_gauss;
  typedef bsta_num_obs<mix_gauss> mix_gauss_type;

  mix_gauss_type f;
  bsta_gauss_sf1 fc1(1,1);
  f.insert(fc1,(1.0f/3.0f));

  bsta_gauss_sf1 fc2(3,2);
  f.insert(fc2,(1.0f/3.0f));

  bsta_gauss_sf1 fc3(15,1);
  f.insert(fc3,(1.0f/3.0f));

  bsta_gauss_sf1 g(6.3333f,39.5556f);

  double dist1 = bvxm_mog_norm<float>::l2_gauss2mix(g,f,false);
  TEST_NEAR("G-F",dist1, 0.2719,0.01);
}

static void test_mog_norm()
{
  test_l2_mog_norm();
  test_gauss2mix();
}

TESTMAIN(test_mog_norm);
