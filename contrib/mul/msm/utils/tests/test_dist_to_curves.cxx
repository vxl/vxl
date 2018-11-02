// This is mul/msm/utils/tests/test_points.cxx
//=======================================================================
//
//  Copyright: (C) 2015 The University of Manchester
//
//=======================================================================
#include <iostream>
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief Test msm_dist_to_curves functions

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <msm/utils/msm_closest_pt.h>
#include <msm/utils/msm_dist_to_curves.h>

//=======================================================================

void test_dist_to_curves()
{
  std::cout << "****************************\n"
           << " Testing msm_dist_to_curves \n"
           << "****************************\n";

  // Create a square
  msm_points points(4);
  points.set_point(0, 0,0);
  points.set_point(1, 1,0);
  points.set_point(2, 1,1);
  points.set_point(3, 0,1);

  msm_curves open_curve(0,3);
  msm_curves closed_curve(0,3,false);

  double d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(2,0));
  TEST_NEAR("Distance to (2,0)",d,1.0,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(2,0.5));
  TEST_NEAR("Distance to (2,0.5)",d,1.0,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(2,1));
  TEST_NEAR("Distance to (2,1)",d,1.0,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(2,2));
  TEST_NEAR("Distance to (2,2)",d,std::sqrt(2.0),1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(0.3,1.1));
  TEST_NEAR("Distance to (0.3,1.1)",d,0.1,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(0.3,0.8));
  TEST_NEAR("Distance to (0.3,0.8)",d,0.2,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(1.1,2.0));
  TEST_NEAR("Distance to (1.1,2.0)",d,std::sqrt(1.01),1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(-1,0.5));
  TEST_NEAR("Distance to (-1,0.5)",d,std::sqrt(1.25),1e-6);

  d=msm_dist_to_curves(points,closed_curve,vgl_point_2d<double>(-1,0.5));
  TEST_NEAR("Distance to (-1,0.5) [Closed]",d,1.0,1e-6);


  msm_line_seg_pt seg1 = msm_closest_seg_pt_on_curve(points,open_curve[0],vgl_point_2d<double>(0.3,-0.5),d);
  TEST("seg1.i0",seg1.i0,0);
  TEST("seg1.i1",seg1.i1,1);
  TEST_NEAR("seg1.alpha",seg1.alpha,0.3,1e-6);

  seg1 = msm_closest_seg_pt_on_curve(points,open_curve[0],vgl_point_2d<double>(0.7,-0.5),d);
  TEST("seg1.i0",seg1.i0,0);
  TEST("seg1.i1",seg1.i1,1);
  TEST_NEAR("seg1.alpha",seg1.alpha,0.7,1e-6);

  seg1 = msm_closest_seg_pt_on_curve(points,open_curve[0],vgl_point_2d<double>(1.1,-0.5),d);
  TEST("seg1.i0",seg1.i0,0);
  TEST("seg1.i1",seg1.i1,1);
  TEST_NEAR("seg1.alpha",seg1.alpha,1.0,1e-6);

  seg1 = msm_closest_seg_pt_on_curve(points,open_curve[0],vgl_point_2d<double>(-0.7,-0.5),d);
  TEST("seg1.i0",seg1.i0,0);
  TEST("seg1.i1",seg1.i1,-1);
  TEST_NEAR("seg1.alpha",seg1.alpha,0.0,1e-6);

  // Create a line
  for (unsigned i=0;i<4;++i)
    points.set_point(i,i,2*i);

  seg1 = msm_closest_seg_pt_on_curve(points,open_curve[0],vgl_point_2d<double>(1.5,3.0),d);
  TEST("seg1.i0",seg1.i0,1);
  TEST("seg1.i1",seg1.i1,2);
  TEST_NEAR("seg1.alpha",seg1.alpha,0.5,1e-6);
}

TESTMAIN(test_dist_to_curves);
