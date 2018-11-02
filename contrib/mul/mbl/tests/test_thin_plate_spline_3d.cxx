// This is mul/mbl/tests/test_thin_plate_spline_3d.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mbl/mbl_thin_plate_spline_3d.h>
#include <vnl/vnl_random.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_distance.h>
#include <testlib/testlib_test.h>

void test_tps3d_at_fixed_points(int n_points)
{
  std::cout<<"Testing with "<<n_points<<" points\n";
  std::vector<vgl_point_3d<double> > pts1(n_points),pts2(n_points);

  vnl_random mz_random;
  mz_random.reseed(1647+2*n_points);

  for (int i=0;i<n_points;++i)
  {
    pts1[i]=vgl_point_3d<double>(mz_random.drand64(),
                                 mz_random.drand64(),
                                 mz_random.drand64());
    pts2[i]=vgl_point_3d<double>(mz_random.drand64(),
                                 mz_random.drand64(),
                                 mz_random.drand64());
  }

  mbl_thin_plate_spline_3d tps;
  tps.build(pts1,pts2);
//  std::cout<<tps<<std::endl;

  for (int i=0;i<n_points;++i)
  {
    std::cout<<"Test point "<<i<<" : ";
    TEST_NEAR("Warped point = target point",vgl_distance(tps(pts1[i]),pts2[i]),0.0,1e-6);
  }

  mbl_thin_plate_spline_3d tps2 = tps;
  TEST("Comparison operator",tps,tps2);

  std::cout<<"Testing set_source_pts(),build() pair\n";
  mbl_thin_plate_spline_3d tps3;
  tps3.set_source_pts(pts1);
  tps3.build(pts2);
  for (int i=0;i<n_points;++i)
  {
    std::cout<<"Test point "<<i<<" : ";
    TEST_NEAR("Warped point = target point",vgl_distance(tps3(pts1[i]),pts2[i]),0.0,1e-6);
  }
}

void test_thin_plate_spline_3d()
{
  std::cout << "**********************************\n"
           << " Testing mbl_thin_plate_spline_3d\n"
           << "**********************************\n";

  for (int i=1;i<=5;++i)
    test_tps3d_at_fixed_points(i);
 }

TESTMAIN(test_thin_plate_spline_3d);
