// This is mul/mbl/tests/test_thin_plate_spline_2d.cxx
#include <vcl_iostream.h>
#include <mbl/mbl_thin_plate_spline_2d.h>
#include <vnl/vnl_random.h>
#include <vgl/vgl_distance.h>
#include <testlib/testlib_test.h>

void test_tps_at_fixed_points(int n_points)
{
  vcl_cout<<"Testing with "<<n_points<<" points\n";
  vcl_vector<vgl_point_2d<double> > pts1(n_points),pts2(n_points);

  vnl_random mz_random;
  mz_random.reseed(1647+2*n_points);

  for (int i=0;i<n_points;++i)
  {
    pts1[i]=vgl_point_2d<double>(mz_random.drand64(),mz_random.drand64());
    pts2[i]=vgl_point_2d<double>(mz_random.drand64(),mz_random.drand64());
  }
//  vsl_print_summary(vcl_cout,pts1);
//  vsl_print_summary(vcl_cout,pts2);

  mbl_thin_plate_spline_2d tps;
  tps.build(pts1,pts2);
//  vcl_cout<<tps<<vcl_endl;

  for (int i=0;i<n_points;++i)
  {
    vcl_cout<<"Test point "<<i<<" : ";
    TEST_NEAR("Warped point = target point",vgl_distance(tps(pts1[i]),pts2[i]),0.0,1e-6);
  }

  mbl_thin_plate_spline_2d tps2 = tps;
  TEST("Comparison operator",tps,tps2);

  vcl_cout<<"Testing set_source_pts(),build() pair\n";
  mbl_thin_plate_spline_2d tps3;
  tps3.set_source_pts(pts1);
  tps3.build(pts2);
  for (int i=0;i<n_points;++i)
  {
    vcl_cout<<"Test point "<<i<<" : ";
    TEST_NEAR("Warped point = target point",vgl_distance(tps3(pts1[i]),pts2[i]),0.0,1e-6);
  }
}

void test_thin_plate_spline_2d()
{
  vcl_cout << "**********************************\n"
           << " Testing mbl_thin_plate_spline_2d\n"
           << "**********************************\n";

  for (int i=1;i<=5;++i)
    test_tps_at_fixed_points(i);
 }

TESTMAIN(test_thin_plate_spline_2d);
