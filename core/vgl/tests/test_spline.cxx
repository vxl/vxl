// Some tests for vgl_spline_*d
// J.L. Mundy August, 2015
#include <testlib/testlib_test.h>
#include <vgl/vgl_cubic_spline_3d.h>
#include <vgl/vgl_cubic_spline_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vgl/vgl_point_2d.h>
#include <vcl_iostream.h>
#include <vcl_fstream.h>
#include <vcl_iostream.h>
#include <vcl_cmath.h>
#include <vpl/vpl.h>

static void test_spline()
{
  vgl_point_3d<double> pm1(1.0, 0.0, 0.0);
  vgl_point_3d<double>  p0(1.0, 3.0, 2.0);
  vgl_point_3d<double>  p1(2.0, 2.0, 4.0);
  vgl_point_3d<double>  p2(2.0, 0.0, 2.0);
  vcl_vector<vgl_point_3d<double> > knots;
  knots.push_back(pm1); knots.push_back(p0);  knots.push_back(p1);   knots.push_back(p2);
   vgl_cubic_spline_3d<double> spl(knots);
  for(double t = 1.0; t<2.1; t+=0.1){
          vgl_point_3d<double> p = spl(t);
          vcl_cout << t << ' ' << p.x() << ' ' << p.y() << ' ' << p.z() << '\n';
  }
  double vxs = (spl(1.5)).x(), vx = 1.5,    ex = vcl_fabs(vxs-vx);
  double vys = (spl(1.5)).y(), vy = 2.8125, ey = vcl_fabs(vys-vy);
  double vzs = (spl(1.5)).z(), vz = 3.25,   ez = vcl_fabs(vzs-vz);
  double error = ex+ey+ez;
  TEST_NEAR("Spline interpolation (open)", error, 0.0, 1e-05);

  vcl_vector<vgl_point_3d<double> > knots2;
  knots2.push_back(pm1);  knots2.push_back(p0); knots2.push_back(p1);
  vgl_cubic_spline_3d<double> spl2(knots2, 0.5, true);
  for(double t = 1.0; t<2; t+=0.1){
    vgl_point_3d<double> p = spl2(t);
    vcl_cout << t << ' ' << p.x() << ' ' << p.y() << ' ' << p.z() << '\n';
  }
  double vxsc = (spl2(1.5)).x(), vxc = 1.5625, exc = vcl_fabs(vxsc-vxc);
  double vysc = (spl2(1.5)).y(), vyc = 2.8125, eyc = vcl_fabs(vysc-vyc);
  double vzsc = (spl2(1.5)).z(), vzc = 3.375,  ezc = vcl_fabs(vzsc-vzc);
  double errorc = exc+eyc+ezc;
  TEST_NEAR("Spline interpolation (closed)", errorc, 0.0, 1e-05);
  // test I/O
  vcl_string path = "./testio.txt";
  vcl_ofstream ostr(path.c_str());
  ostr << spl;
  ostr.close();
  vcl_ifstream istr(path.c_str());
  vgl_cubic_spline_3d<double> io_spl;
  istr >> io_spl;
  bool good = io_spl == spl;
  TEST("Spline 3d I/O", good, true);
  vpl_unlink(path.c_str());

  // test tangent
  vgl_vector_3d<double> tan = spl.tangent(1.5);
  vcl_cout << tan << '\n';
  double dx = 0.401288, dy = -0.441404, dz = 0.802577;
  double etx = vcl_fabs(dx-tan.x()), ety = vcl_fabs(dy-tan.y()), etz = vcl_fabs(dz-tan.z());
  double etan = etx + ety + etz;
  TEST_NEAR("spline tangent", etan, 0.0, 0.001);

  // 2-d spline tests
  vgl_point_2d<double> pm12(1.0, 0.0);
  vgl_point_2d<double>  p02(1.0, 3.0);
  vgl_point_2d<double>  p12(2.0, 2.0);
  vgl_point_2d<double>  p22(2.0, 0.0);
  vcl_vector<vgl_point_2d<double> > knots2d;
  knots2d.push_back(pm12); knots2d.push_back(p02);  knots2d.push_back(p12);   knots2d.push_back(p22);
  vgl_cubic_spline_2d<double> spl2d(knots2d);
  double vxs2d = (spl2d(1.5)).x(), vx2d = 1.5,    ex2d = vcl_fabs(vxs2d-vx2d);
  double vys2d = (spl2d(1.5)).y(), vy2d = 2.8125, ey2d = vcl_fabs(vys2d-vy2d);
  
  double error2d = ex2d+ey2d;
  TEST_NEAR("2d spline interpolation  (open)", error2d, 0.0, 1e-05);
  // test 2d I/O
  vcl_string path2d = "./testio.txt";
  vcl_ofstream ostr2d(path2d.c_str());
  ostr2d << spl2d;
  ostr2d.close();
  vcl_ifstream istr2d(path2d.c_str());
  vgl_cubic_spline_2d<double> io_spl2d;
  istr2d >> io_spl2d;
  good = io_spl2d == spl2d;
  TEST("Spline 2d I/O", good, true);
  vpl_unlink(path2d.c_str());
  // test tangent
  vgl_vector_2d<double> tan2d = spl2d.tangent(1.5);
  vcl_cout << tan2d << '\n';
  dx =0.672684; dy=-0.73993;
  double etx2d = vcl_fabs(dx-tan2d.x()), ety2d = vcl_fabs(dy-tan2d.y());
  double etan2d = etx2d + ety2d;
  TEST_NEAR("spline tangent", etan2d, 0.0, 0.001);
}

TESTMAIN(test_spline);
