#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vnl/vnl_test.h>
#include <mbl/mbl_thin_plate_spline_2d.h>
#include <mbl/mbl_mz_random.h>
#include <vgl/vgl_distance.h>

void test_tps_at_fixed_points(int n_points)
{
  vcl_cout<<"Testing with "<<n_points<<vcl_endl;
  vcl_vector<vgl_point_2d<double> > pts1(n_points),pts2(n_points);

  mbl_mz_random mz_random;
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
    TEST("Warped point = target point",vgl_distance(tps(pts1[i]),pts2[i])<1e-6,true);
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
    TEST("Warped point = target point",vgl_distance(tps3(pts1[i]),pts2[i])<1e-6,true);
  }
}

void test_thin_plate_spline_2d()
{
  vcl_cout << "**********************************\n";
  vcl_cout << " Testing mbl_thin_plate_spline_2d\n";
  vcl_cout << "**********************************\n";

  for (int i=1;i<=5;++i)
    test_tps_at_fixed_points(i);
 }

TESTMAIN(test_thin_plate_spline_2d);
