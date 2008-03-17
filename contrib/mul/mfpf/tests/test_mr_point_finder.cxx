// This is mul/mfpf/tests/test_mr_point_finder.cxx
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

#include <vcl_iostream.h>
#include <vsl/vsl_binary_loader.h>
#include <mfpf/mfpf_add_all_loaders.h>
#include <mfpf/mfpf_norm_corr2d.h>
#include <mfpf/mfpf_norm_corr2d_builder.h>
#include <vil/vil_bilin_interp.h>
#include <mfpf/mfpf_mr_point_finder.h>
#include <mfpf/mfpf_mr_point_finder_builder.h>
#include <vimt/vimt_image_pyramid.h>
#include <vimt/vimt_gaussian_pyramid_builder_2d.h>

//=======================================================================

void test_mr_point_finder_search(mfpf_mr_point_finder_builder& b)
{
  vcl_cout<<"Testing building and search."<<vcl_endl;

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

  vcl_cout<<"Built model: "<<pf<<vcl_endl;

  mfpf_pose pose0(p0,u);
  mfpf_pose pose1(p1,u1);

  mfpf_pose new_pose;
  pf.search(image_pyr,pose1,new_pose);

  vcl_cout<<"Initial pose: "<<pose1<<vcl_endl;
  vcl_cout<<"Ideal   pose: "<<pose0<<vcl_endl;
  vcl_cout<<"Final   pose: "<<new_pose<<vcl_endl;

  TEST_NEAR("search: Correct location",
            (new_pose.p()-p0).length(),0.0,0.1);

  vcl_vector<mfpf_pose> poses;
  vcl_vector<double> fits;
  pf.multi_search(image_pyr,pose1,poses,fits);

  vcl_cout<<"Number of responses (no pruning) = "
          <<poses.size()<<vcl_endl;

  pf.multi_search_and_prune(image_pyr,pose1,poses,fits,-2);

  vcl_cout<<"Number of responses (with pruning) = "
          <<poses.size()<<vcl_endl;

  for (unsigned i=0;i<poses.size();++i)
    vcl_cout<<i<<") "<<poses[i]<<" fit: "<<fits[i]<<vcl_endl;
}

void test_mr_point_finder()
{
  vcl_cout << "**************************\n"
           << " Testing mfpf_mr_point_finder\n"
           << "**************************\n";

  mfpf_norm_corr2d_builder nc_builder;
  nc_builder.set_kernel_size(7,7);
  nc_builder.set_search_area(4,4);
  nc_builder.set_search_scale_range(3,1.1);
  nc_builder.set_search_angle_range(3,0.05);

  mfpf_mr_point_finder_builder mr_builder;
  mr_builder.set(nc_builder,3,1.0,2.0);

  test_mr_point_finder_search(mr_builder);

}

TESTMAIN(test_mr_point_finder);
