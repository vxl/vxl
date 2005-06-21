//:
// \file
// \author Joseph Mundy
// \date  March 28, 2003

#include <testlib/testlib_test.h>
#include <vcl_cmath.h>
#include <vcl_vector.h>
#include <vcl_iostream.h>
#include <vgl/vgl_vector_2d.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_conic.h>
#include <vgl/vgl_conic_segment_2d.h>
#include <vgl/algo/vgl_conic_2d_regression.h>
#include <vgl/algo/vgl_fit_conics_2d.h>
#if 0
static void test_conic_segment_methods()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(1.0,2.0);
  vgl_conic<double> l(1.0,-1.0,0.0);
  vcl_cout << "line angle " << l.slope_degrees() << '\n';
  TEST_NEAR("line angle", l.slope_degrees(), 45, 1e-04);
  vgl_conic_segment_2d<double> seg(p0,p1);
  double angle = seg.slope_degrees();
  TEST_NEAR("line angle", seg.slope_degrees(),63.43495 , 1e-04);
  vgl_vector_2d<double> dir = seg.direction(), norm = seg.normal();
  vcl_cout << "segment angle = " << angle << '\n'
           << "direction " << dir << '\n'
           << "normal " << norm << '\n';
}
#endif

static void test_conic_regression()
{
  vcl_cout << "Testing conic regression (unit circle) \n";
  vgl_point_2d<double> p0(1.0, 0), p1(0.707, 0.707), p2(0.0, 1.0),
	  p3(-0.707, 0.707), p4(-1.0, 0.0), p5(-0.707, -0.707), p6(0.0, -1.0),
	  p7(0.707, -0.707);
  vgl_conic_2d_regression<double> reg;
  reg.add_point(p0);
  reg.add_point(p1);
  reg.add_point(p2);
  reg.add_point(p3);
  reg.add_point(p4);
  reg.add_point(p5);
  reg.add_point(p6);
  reg.add_point(p7);
 

  reg.fit();
  vcl_cout << "algebraic fitting error " << reg.get_rms_algebraic_error() << '\n';
  vcl_cout << "sampson fitting error " << reg.get_rms_sampson_error() << '\n'
           << "fitted conic " << reg.conic() << '\n';

  TEST_NEAR("unit circle", reg.get_rms_sampson_error(), 0.0, 1e-04);

   
  vgl_point_2d<double> q0(1.414, 1.414), q1(-0.707, 0.707),
    q2(-1.414, -1.414), q3(0.707, -0.707), q4(0, 1.26472), q5(0, -1.26472),
    q6(1.26472, 0), q7(-1.26472, 0);
  reg.clear_points();
  reg.add_point(q0);
  reg.add_point(q1);
  reg.add_point(q2);
  reg.add_point(q3);
  reg.add_point(q4);
  reg.add_point(q5);
  reg.add_point(q6);
  reg.add_point(q7);
  reg.fit();
  vcl_cout << "algebraic fitting error " << reg.get_rms_algebraic_error() << '\n';
  vcl_cout << "sampson fitting error " << reg.get_rms_sampson_error() << '\n'
           << "fitted conic " << reg.conic() << '\n';
  TEST_NEAR("2:1 at 45 deg", reg.get_rms_sampson_error(), 0.0, 1e-06);
  reg.clear_points();

reg.add_point(vgl_point_2d<double>(3.52074e-012,0.0));
reg.add_point(vgl_point_2d<double>(0.0151927,0.173651));
reg.add_point(vgl_point_2d<double>(0.0603082,0.342022));
reg.add_point(vgl_point_2d<double>(0.133976,0.500002));
reg.add_point(vgl_point_2d<double>(0.233957,0.642789));
reg.add_point(vgl_point_2d<double>(0.357214,0.766046));
reg.add_point(vgl_point_2d<double>(0.500002,0.866026));
reg.add_point(vgl_point_2d<double>(0.657981,0.939693));
reg.add_point(vgl_point_2d<double>(0.826353,0.984808));
reg.add_point(vgl_point_2d<double>(1,1));
reg.add_point(vgl_point_2d<double>(1.17365,0.984808));
reg.add_point(vgl_point_2d<double>(1.34202,0.939692));
reg.add_point(vgl_point_2d<double>(1.5,0.866025));
reg.add_point(vgl_point_2d<double>(1.64279,0.766044));
reg.add_point(vgl_point_2d<double>(1.76604,0.642787));
reg.add_point(vgl_point_2d<double>(1.86603,0.5));
reg.add_point(vgl_point_2d<double>(1.93969,0.34202));
reg.add_point(vgl_point_2d<double>(1.98481,0.173648));
reg.add_point(vgl_point_2d<double>(2,0.0));
  reg.fit();
  vcl_cout << "algebraic fitting error " << reg.get_rms_algebraic_error() << '\n';
  vcl_cout << "sampson fitting error " << reg.get_rms_sampson_error() << '\n'
           << "fitted conic " << reg.conic() << '\n';
  TEST_NEAR("semi-circle centered on (1,0) ", reg.get_rms_sampson_error(), 0.0, 1e-05);
  reg.clear_points();

}

//A parametric form of the unit circle for testing ( s = 10 degree intervals)
static void unit_circle(double s, double x0, double y0, double& x, double& y)
{
  double theta = static_cast<double>(s)*3.14159/18;
  double c = vcl_cos(theta), si = vcl_sin(theta);
  x = x0 + c; y = y0 + si;
}

static void test_fit_simple_chain()
{

  vcl_vector<vgl_point_2d<double> > curve;

  // Two segments from a unit circle forming a kind of sine wave

  //first segment above horizontal axis centered at (1,0) scan -pi to 0
  vgl_fit_conics_2d<double> fitter;
  for (int i = 18; i>=0; --i)
  {
    double x, y;
    unit_circle(i, 1.0, 0.0, x, y);
    vgl_point_2d<double> p(x, y);
    fitter.add_point(p);
  }
  //Second segment below horizontal axis centered at (3,0) scan pi to 360
  
  for (int i = 19; i<=36; ++i)
  {
    double x, y;
    unit_circle(i, 3.0, 0.0, x, y);
    vgl_point_2d<double> p(x, y);
    fitter.add_point(p);
  }

  fitter.fit();
  vcl_vector<vgl_conic_segment_2d<double> >& segs = fitter.get_conic_segs();
  vcl_cout << "\nCurve fit Produced the following conic segments\n";
  for (vcl_vector<vgl_conic_segment_2d<double> >::iterator sit = segs.begin();
       sit != segs.end(); sit++)
    vcl_cout << *sit << '\n';

  TEST("Number of conic segments ", segs.size(), 2);

}

static void test_fit_conics_2d()
{
  //  test_conic_segment_methods();
  test_conic_regression();
  test_fit_simple_chain();
}

TESTMAIN(test_fit_conics_2d);
