#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_utility.h>
#include <vcl_cmath.h>
#include <vnl/vnl_test.h>
#include <mbl/mbl_clamped_plate_spline_2d.h>
#include <mbl/mbl_mz_random.h>
#include <vgl/vgl_distance.h>

void test_cps_at_fixed_points(int n_points)
{
  vcl_cout<<"Testing with "<<n_points<<vcl_endl;
  vcl_vector<vgl_point_2d<double> > pts1(n_points),pts2(n_points);

  mbl_mz_random mz_random;
  mz_random.reseed(1647+2*n_points);

  for (int i=0;i<n_points;++i)
  {
    pts1[i]=vgl_point_2d<double>(0.5*mz_random.drand64(),0.5*mz_random.drand64());
    pts2[i]=vgl_point_2d<double>(0.5*mz_random.drand64(),0.5*mz_random.drand64());
  }
//  vsl_print_summary(vcl_cout,pts1);
//  vsl_print_summary(vcl_cout,pts2);

  mbl_clamped_plate_spline_2d cps;
  cps.build(pts1,pts2);
//  vcl_cout<<cps<<vcl_endl;

  for (int i=0;i<n_points;++i)
  {
    vcl_cout<<"Test point "<<i<<" : ";
    TEST("Warped point = target",vgl_distance(cps(pts1[i]),pts2[i])<1e-6,true);
  }

  mbl_clamped_plate_spline_2d cps2 = cps;
  TEST("Comparison operator",cps,cps2);

  vcl_cout<<"Testing set_source_pts(),build() pair\n";
  mbl_clamped_plate_spline_2d cps3;
  cps3.set_source_pts(pts1);
  cps3.build(pts2);
  for (int i=0;i<n_points;++i)
  {
    vcl_cout<<"Test point "<<i<<" : ";
    TEST("Warped point = target",vgl_distance(cps3(pts1[i]),pts2[i])<1e-6,true);
  }

  vgl_point_2d<double> p1(0,1);
  TEST("(0,1) unchanged",vgl_distance(cps3(p1),p1)<1e-6,true);

  vgl_point_2d<double> p2(0,-1);
  TEST("(0,-1) unchanged",vgl_distance(cps3(p2),p2)<1e-6,true);

  vgl_point_2d<double> p3(1,0);
  TEST("(1,0) unchanged",vgl_distance(cps3(p3),p3)<1e-6,true);
}

void test_clamped_plate_spline_2d()
{
  vcl_cout << "**********************************\n";
  vcl_cout << " Testing mbl_clamped_plate_spline_2d\n";
  vcl_cout << "**********************************\n";

  for (int i=1;i<=4;++i)
    test_cps_at_fixed_points(i);
 }

TESTMAIN(test_clamped_plate_spline_2d);
