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
#include <vgl/vgl_line_2d.h>
#include <vgl/vgl_line_segment_2d.h>
#include <vgl/algo/vgl_line_2d_regression.h>
#include <vgl/algo/vgl_fit_lines_2d.h>

static void test_line_segment_methods()
{
  vgl_point_2d<double> p0(0.0,0.0), p1(1.0,2.0);
  vgl_line_2d<double> l(1.0,-1.0,0.0);
  vcl_cout << "line angle " << l.slope_degrees() << '\n';
  TEST_NEAR("line angle", l.slope_degrees(), 45, 1e-04);
  vgl_line_segment_2d<double> seg(p0,p1);
  double angle = seg.slope_degrees();
  TEST_NEAR("line angle", seg.slope_degrees(),63.43495 , 1e-04);
  vgl_vector_2d<double> dir = seg.direction(), norm = seg.normal();
  vcl_cout << "segment angle = " << angle << '\n'
           << "direction " << dir << '\n'
           << "normal " << norm << '\n';
}

static void test_line_2d_regression()
{
  vcl_cout << "Testing line regression\n";
  vgl_point_2d<double> p0(0.1,0.9), p1(0.5,1.6), p2(1.2,2.0);
  vgl_line_2d_regression<double> reg;
  reg.increment_partial_sums(p0.x(), p0.y());
  reg.increment_partial_sums(p1.x(), p1.y());
  reg.increment_partial_sums(p2.x(), p2.y());
  reg.fit();
  vcl_cout << "fitting error " << reg.get_rms_error() << '\n'
           << "fitted line " << reg.get_line() << '\n';
  double dist = vcl_fabs(0.1-reg.get_rms_error());
  TEST_NEAR("identity", dist, 0.0, 1e-04);
}

static void test_fit_simple_chain()
{
  vcl_vector<vgl_point_2d<double> > curve;
  int npts= 15;
  vgl_fit_lines_2d<double> fitter;
  //A segment with a slope of 2
  for (int i = 0; i<npts; i++)
    {
      vgl_point_2d<double> p(i,2*i);
      fitter.add_point(p);
    }
  //A segment with a slope of -1
  for (int i = 0; i<npts; i++)
    {
      vgl_point_2d<double> p(i+npts,(2*(npts-1)-i));
      fitter.add_point(p);
    }
  //Add a segment with a slope of +1 but too short
  vgl_point_2d<double> p0(2*npts, npts);
  vgl_point_2d<double> p1(2*npts+1, npts+1);
  fitter.add_point(p0);fitter.add_point(p1);
  vcl_vector<vgl_point_2d<double> >& c = fitter.get_points();
  for (vcl_vector<vgl_point_2d<double> >::iterator pit = c.begin();
       pit != c.end(); pit++)
     vcl_cout << *pit << '\n';
  fitter.fit();
  vcl_vector<vgl_line_segment_2d<double> >& segs = fitter.get_line_segs();
  for (vcl_vector<vgl_line_segment_2d<double> >::iterator sit = segs.begin();
       sit != segs.end(); sit++)
    vcl_cout << *sit << '\n';
  TEST("no of line segments ", segs.size(), 2);
  TEST("last endpoint x", segs[1].point2().x(), 29);
}

static void test_fit_lines_2d()
{
  test_line_segment_methods();
  test_line_2d_regression();
  test_fit_simple_chain();
}

TESTMAIN(test_fit_lines_2d);
