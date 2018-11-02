// This is mul/mfpf/tests/test_pose_predictor.cxx
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test mfpf_pose_predictor
//=======================================================================
//
//  Copyright: (C) 2009 The University of Manchester
//
//=======================================================================

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_pose_predictor.h>
#include <mfpf/mfpf_pose_predictor_builder.h>
#include <vil/vil_crop.h>
#include <vil/algo/vil_gauss_filter.h>

//=======================================================================

void test_pose_predictor_search(mfpf_pose_predictor& pp)
{
  std::cout<<"========== Testing Search under "
          <<pp.pose_type()<<" =====\n";

  // Create a test image
  vimt_image_2d_of<float> image(21,21);
  image.image().fill(0);

  // Add a 5x5 square in the centre
  vil_crop(image.image(),8,5,8,5).fill(99.9f);

  // Add a 3x5 rectangle in the centre
  vil_crop(image.image(),8,5,9,3).fill(180.0f);

  vil_gauss_filter_2d(image.image(),image.image(),0.5,3);

  mfpf_pose pose0(10,10,1,0);

  mfpf_pose_predictor_builder pp_builder;
  pp_builder.set_sampling(pp);
  pp_builder.set_n_per_eg(75);

  pp_builder.clear(1);
  pp_builder.add_example(image,pose0);
  pp_builder.build(pp);

  std::cout<<"Built model: "<<pp<<std::endl;

  mfpf_pose new_pose;

  for (unsigned j=9;j<=11;++j)
    for (unsigned i=8;i<=12;++i)
    {
      mfpf_pose pose1(i,j,1,0);
      double d0 = mfpf_max_sqr_diff(pose1,pose0,1.0);

      pp.new_pose(image,pose1,new_pose);
//       pp.new_pose(image,new_pose,new_pose);
      std::cout<<i<<","<<j<<" : "<<new_pose<<std::endl;
      double d = mfpf_max_sqr_diff(new_pose,pose0,1.0);
      double r = (new_pose.p()-pose0.p()).length();
      TEST_NEAR("Test at displaced pt",r,0,0.9);
      if (d0>1e-6)
        TEST("Prediction improved accuracy",d<d0,true);
    }

  pp.new_pose(image,pose0,new_pose);
  std::cout<<"pose0: "<<pose0<<std::endl;
  std::cout<<"New pose: "<<new_pose<<std::endl;
  TEST("Pose unchanged at true point",
       mfpf_max_sqr_diff(new_pose,pose0,1.0)<0.2, true);
}

void test_pose_predictor()
{
  std::cout << "**************************\n"
           << " Testing mfpf_pose_predictor\n"
           << "**************************\n";

  mfpf_add_all_loaders();

  mfpf_pose_predictor pp;
  pp.set_as_ellipse(4.5,4.5,1);
  std::cout<<"Sampler: "<<pp<<std::endl;

  pp.set_pose_type(translation);
  test_pose_predictor_search(pp);

  pp.set_pose_type(rigid);
  test_pose_predictor_search(pp);

  pp.set_pose_type(zoom);
  test_pose_predictor_search(pp);

  pp.set_pose_type(similarity);
  test_pose_predictor_search(pp);

  {
    // Test binary load and save

    vsl_b_ofstream bfs_out("test_pose_predictor.bvl.tmp");
    TEST ("Created test_pose_predictor.bvl.tmp for writing", (!bfs_out), false);
    vsl_b_write(bfs_out, pp);
    bfs_out.close();

    mfpf_pose_predictor pp_in;

    vsl_b_ifstream bfs_in("test_pose_predictor.bvl.tmp");
    TEST ("Opened test_pose_predictor.bvl.tmp for reading", (!bfs_in), false);
    vsl_b_read(bfs_in, pp_in);
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();
    std::cout<<pp<<std::endl
            <<pp_in<<std::endl;
    TEST("Loaded==Saved",pp_in,pp);
  }

  vsl_delete_all_loaders();
}

TESTMAIN(test_pose_predictor);
