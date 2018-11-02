// This is mul/mfpf/tests/test_mr_point_finder.cxx
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_mr_point_finder
//=======================================================================
//
//  Copyright: (C) 2008 The University of Manchester
//
//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_norm_corr2d.h>
#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <mfpf/mfpf_region_finder_builder.h>
#include <mfpf/mfpf_sad_vec_cost_builder.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_vector_2d.h>
#include <mfpf/mfpf_mr_point_finder.h>
#include <mfpf/mfpf_mr_point_finder_builder.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

//=======================================================================

void test_mr_point_finder_search(mfpf_mr_point_finder_builder& b,
                                 double d_thresh)
{
  std::cout<<"Testing building and search."<<std::endl;

  mfpf_mr_point_finder pf;

  // Create a test image
  vimt_image_2d_of<float> image(100,100);
  image.image().fill(0);

  // Add a cross in the centre
  for (unsigned i=40;i<=60;++i)
  {
    image.image()(i,48)=99;
    image.image()(i,52)=99;
    image.image()(48,i)=99;
    image.image()(52,i)=99;
  }

  vimt_image_pyramid image_pyr;
  vimt_gaussian_pyramid_builder_2d<float> pyr_builder;
  pyr_builder.build(image_pyr,image);

  vgl_point_2d<double> p0(50,50), p1(57,42);
  vgl_vector_2d<double> u(1,0),u1(1.03,0.02);

  b.clear(1);
  b.add_example(image_pyr,p0,u);
  b.build(pf);

  std::cout<<"Built model: "<<pf<<std::endl;

  mfpf_pose pose0(p0,u);
  mfpf_pose pose1(p1,u1);

  mfpf_pose new_pose;
  pf.search(image_pyr,pose1,new_pose);

  std::cout<<"Initial pose: "<<pose1<<std::endl
          <<"Ideal   pose: "<<pose0<<std::endl
          <<"Final   pose: "<<new_pose<<std::endl;

  TEST_NEAR("search: Correct location",
            (new_pose.p()-p0).length(),0.0,d_thresh);

  std::vector<mfpf_pose> poses;
  std::vector<double> fits;
  pf.multi_search(image_pyr,pose1,poses,fits);

  std::cout<<"Number of responses (no pruning) = "
          <<poses.size()<<std::endl;

  pf.multi_search_and_prune(image_pyr,pose1,poses,fits,-2);

  std::cout<<"Number of responses (with pruning) = "
          <<poses.size()<<std::endl;

  for (unsigned i=0;i<poses.size();++i)
    std::cout<<i<<") "<<poses[i]<<" fit: "<<fits[i]<<std::endl;
}

void test_mr_point_finder()
{
  std::cout << "**************************\n"
           << " Testing mfpf_mr_point_finder\n"
           << "**************************\n";

  {
    std::cout<<"*** Test using mfpf_norm_corr2d_builder ***"<<std::endl;

    mfpf_norm_corr2d_builder nc_builder;
    nc_builder.set_kernel_size(7,7);
    nc_builder.set_search_area(4,4);
    nc_builder.set_search_scale_range(3,1.1);
    nc_builder.set_search_angle_range(3,0.05);

    mfpf_mr_point_finder_builder mr_builder;
    mr_builder.set(nc_builder,3,1.0,2.0);

    test_mr_point_finder_search(mr_builder,0.1);
  }

  {
    std::cout<<"*** Test using mfpf_region_finder_builder ***"<<std::endl;

    mfpf_sad_vec_cost_builder sad_vec_cost_builder;

    mfpf_region_finder_builder rf_builder;
    rf_builder.set_as_box(7,7,sad_vec_cost_builder);

    rf_builder.set_search_area(4,4);
    rf_builder.set_search_scale_range(3,1.1);
    rf_builder.set_search_angle_range(3,0.05);

    std::cout<<"Base builder: "<<rf_builder<<std::endl;

    mfpf_mr_point_finder_builder mr_builder;
    mr_builder.set(rf_builder,3,1.0,2.0);

    // Sum of absolutes gives slightly less accurate result
    // than normalised correlation in this case.
    test_mr_point_finder_search(mr_builder,0.25);
  }
}

TESTMAIN(test_mr_point_finder);
