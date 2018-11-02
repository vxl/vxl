// This is mul/msm/tests/test_points.cxx
//=======================================================================
//
//  Copyright: (C) 2008 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief test msm_points

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vsl/vsl_binary_loader.h>
#include <msm/msm_points.h>
#include <vgl/vgl_point_2d.h>

//=======================================================================

void test_points()
{
  std::cout << "***********************\n"
           << " Testing msm_points\n"
           << "***********************\n";

  msm_points points;
  points.vector().set_size(10);
  for (unsigned i=0;i<10;++i) points.vector()[i]=i;

  std::cout<<points<<std::endl;

  TEST("Number of points",points.size(),5);
  TEST_NEAR("First point",
            (points[0]-vgl_point_2d<double>(0,1)).length(),0,1e-6);

  std::vector<vgl_point_2d<double> > pts0;
  points.get_points(pts0);
  TEST("Number of points",pts0.size(),points.size());

  msm_points points1;
  points1.set_points(pts0);
  TEST("Set points",points1,points);

  std::cout<<"CoG: "<<points.cog()<<std::endl;
  TEST_NEAR("Centre of Gravity",
            (points.cog()-vgl_point_2d<double>(4,5)).length(),0,1e-6);

  points1.translate_by(0.5,0.7);
  TEST_NEAR("translate_by",
            (points1.cog()-vgl_point_2d<double>(4.5,5.7)).length(),0,1e-6);

  std::cout<<"Scale: "<<points1.scale()<<std::endl;
  TEST_NEAR("scale()",points1.scale(),4,1e-6);

  points1.scale_by(1.5);
  TEST_NEAR("scale_by()",points1.scale(),6,1e-6);

  vgl_point_2d<double> cog;
  double s;
  points.get_cog_and_scale(cog,s);
  TEST_NEAR("CoG",(cog-points.cog()).length(),0,1e-6);
  TEST_NEAR("scale",s,points.scale(),1e-6);

  vgl_point_2d<double> b_lo,b_hi;
  points.get_bounds(b_lo,b_hi);
  TEST_NEAR("Lower bound",
            (b_lo-vgl_point_2d<double>(0,1)).length(),0,1e-6);
  TEST_NEAR("Upper bound",
            (b_hi-vgl_point_2d<double>(8,9)).length(),0,1e-6);

  {
    std::cout<<"Test binary load and save"<<std::endl;

    vsl_b_ofstream bfs_out("test_points.bvl.tmp");
    TEST ("Created test_points.bvl.tmp for writing",
          (!bfs_out), false);
    vsl_b_write(bfs_out, points);
    bfs_out.close();

    msm_points points_in;

    vsl_b_ifstream bfs_in("test_points.bvl.tmp");
    TEST ("Opened test_points.bvl.tmp for reading",
          (!bfs_in), false);
    vsl_b_read(bfs_in, points_in);
    TEST ("Finished reading file successfully", (!bfs_in), false);
    bfs_in.close();

    TEST("Loaded==Saved",points_in,points);
  }

  {
    std::cout<<"Test text load and save"<<std::endl;
    std::string text_path("test_points.txt.tmp");
    TEST("write_text_file",points.write_text_file(text_path),true);

    msm_points points_in;
    TEST("read_text_file",points_in.read_text_file(text_path),true);

    TEST("Loaded==Saved",points_in,points);
  }
}

TESTMAIN(test_points);
