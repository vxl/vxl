// This is brl/bbas/bdgl/tests/test_curve_algs.cxx
#include <vcl_iostream.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <vsol/vsol_point_2d.h>
#include <bdgl/bdgl_curve_algs.h>
#include <testlib/testlib_test.h>

static void test_curve_algs()
{
  // we want to test the methods on bdgl_curve_algs
  vcl_cout << "Testing nearest point\n";
  // construct and edgel chain at 45 degrees
  int N = 10;
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
  for (int i = 0; i< N; i++)
    ec->add_edgel(vdgl_edgel(double(i),double(i)));
  int ic = bdgl_curve_algs::closest_point(ec, 7, 3);
  TEST("Closest point index for (7, 3)  = 5", ic, 5);

  //construct a straight curve vertically at x=1.0
  vsol_point_2d_sptr p0 = new vsol_point_2d(1.0,0.0);
  vsol_point_2d_sptr p1 = new vsol_point_2d(1.0,5.0);

  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(p0, p1);
  vgl_line_2d<double> l(0.0, 1.0, -2.1);
  vcl_vector<vgl_point_2d<double> > pts;
  bool good = bdgl_curve_algs::intersect_line(dc, l, pts);
  TEST("Num Intersection points = 1", pts.size(), 1);
  TEST_NEAR("Intersection point = (1,2.1) (x)", pts[0].x(), 1.0, 1e-9);
  TEST_NEAR("Intersection point = (1,2.1) (y)", pts[0].y(), 2.1, 1e-9);
  // test fast curve intersection
  pts.clear();
  good = bdgl_curve_algs::intersect_line_fast(dc, l, pts);
  TEST("(fast) Num Intersection points = 1", pts.size(), 1);
  TEST_NEAR("(fast) Intersection point = (1,2.1) (x)", pts[0].x(), 1.0, 1e-9);
  TEST_NEAR("(fast) Intersection point = (1,2.1) (y)", pts[0].y(), 2.1, 1e-9);
  //construct a  curve that will have two intersections

  vdgl_edgel_chain_sptr ec1 = new vdgl_edgel_chain();
  for (int i = 0; i< 5; i++)
    ec1->add_edgel(vdgl_edgel(1.0,double(i)));
  ec1->add_edgel(vdgl_edgel(1.5, 4.0));
  for (int i = 0; i< 5; i++)
    ec1->add_edgel(vdgl_edgel(2.0,double(4-i)));
  int N1 = ec1->size();
  for (int j = 0; j<N1; j++)
    vcl_cout << (*ec1)[j] << '\n';
  vdgl_interpolator_sptr intp = new vdgl_interpolator_linear(ec1);
  vdgl_digital_curve_sptr dc1 = new vdgl_digital_curve(intp);
  pts.clear();
  good = bdgl_curve_algs::intersect_line(dc1, l, pts);
  TEST("Num Intersection points = 2", pts.size(), 2);
  if (good)
    vcl_cout << " p[0] " << pts[0] << " p[1] " << pts[1] << '\n';
  // test fast curve intersection
  pts.clear();
  good = bdgl_curve_algs::intersect_line_fast(dc1, l, pts);
  TEST("(fast) Num Intersection points = 2", pts.size(), 2);
  vcl_cout << "points size: "<<pts.size() << vcl_endl;
  if (good)
    vcl_cout << " p[0] " << pts[0] << " p[1] " << pts[1] << '\n';
  //test closest point
  double xc=0, yc=0;
  good =  bdgl_curve_algs::closest_point(dc, 0.5, 2.0, xc, yc);
  vcl_cout << "Closest Point to (0.5, 2.0)=(" << xc << ' ' << yc << ")\n";
}

TESTLIB_DEFINE_MAIN(test_curve_algs);
