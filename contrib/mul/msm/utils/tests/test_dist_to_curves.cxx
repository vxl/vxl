// This is mul/msm/utils/tests/test_points.cxx
//=======================================================================
//
//  Copyright: (C) 2015 The University of Manchester
//
//=======================================================================
#include <testlib/testlib_test.h>
//:
// \file
// \author Tim Cootes
// \brief Test msm_dist_to_curves functions

#include <vcl_iostream.h>
#include <msm/utils/msm_dist_to_curves.h>

//=======================================================================

void test_dist_to_curves()
{
  vcl_cout << "****************************\n"
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
  TEST_NEAR("Distance to (2,2)",d,vcl_sqrt(2.0),1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(0.3,1.1));
  TEST_NEAR("Distance to (0.3,1.1)",d,0.1,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(0.3,0.8));
  TEST_NEAR("Distance to (0.3,0.8)",d,0.2,1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(1.1,2.0));
  TEST_NEAR("Distance to (1.1,2.0)",d,vcl_sqrt(1.01),1e-6);

  d=msm_dist_to_curves(points,open_curve,vgl_point_2d<double>(-1,0.5));
  TEST_NEAR("Distance to (-1,0.5)",d,vcl_sqrt(1.25),1e-6);

  d=msm_dist_to_curves(points,closed_curve,vgl_point_2d<double>(-1,0.5));
  TEST_NEAR("Distance to (-1,0.5) [Closed]",d,1.0,1e-6);


}

TESTMAIN(test_dist_to_curves);
