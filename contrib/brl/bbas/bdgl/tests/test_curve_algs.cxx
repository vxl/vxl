// This is brl/bbas/bdgl/tests/test_curve_algs.cxx
#include <vcl_iostream.h>
#include <vdgl/vdgl_edgel_chain_sptr.h>
#include <vdgl/vdgl_edgel_chain.h>
#include <vdgl/vdgl_interpolator_linear.h>
#include <vdgl/vdgl_digital_curve.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_line_2d.h>
#include <bdgl/bdgl_curve_algs.h>

#define Assert(x) { vcl_cout << #x "\t\t\t test "; \
  if (x) { ++success; vcl_cout << "PASSED\n"; } else { ++failures; vcl_cout << "FAILED\n"; } }


int main(int, char **)
{
  int success=0, failures=0;

  // we want to test the methods on bdgl_curve_algs
  vcl_cout << "Testing nearest point\n";
  // construct and edgel chain at 45 degrees
  int N = 10;
  vdgl_edgel_chain_sptr ec = new vdgl_edgel_chain();
  for (int i = 0; i< N; i++)
    ec->add_edgel(vdgl_edgel((double)i,(double)i));
  int ic = bdgl_curve_algs::closest_point(ec, 7, 4);
  vcl_cout << "Closest point index for (7, 4)  = " << ic << vcl_endl;
  Assert(ic==5);

  //construct a straight curve vertically at x=1.0
  vsol_point_2d_sptr p0 = new vsol_point_2d(1.0,0.0);
  vsol_point_2d_sptr p1 = new vsol_point_2d(1.0,5.0);

  vdgl_digital_curve_sptr dc = new vdgl_digital_curve(*p0, *p1);
  vgl_line_2d<double> l(0.0, 1.0, -2.1);
  vcl_vector<vgl_point_2d<double> > pts;
  bool good = bdgl_curve_algs::intersect_line(dc, l, pts);
  if(good)
    {
      vcl_cout << "Num Intersection points = " << pts.size() << "\n";
      vcl_cout << " p" << pts[0] << "\n";
    }
  Assert(pts.size()==1);
  //construct a  curve that will have two intersections

  vdgl_edgel_chain_sptr ec1 = new vdgl_edgel_chain();
  for (int i = 0; i< 5; i++)
    ec1->add_edgel(vdgl_edgel(1.0,(double)i));
  ec1->add_edgel(vdgl_edgel(1.5, 4.0));
  for (int i = 0; i< 5; i++)
    ec1->add_edgel(vdgl_edgel(2.0,(double)(4.0-i)));
  int N1 = ec1->size();
  for(int j = 0; j<N1; j++)
    vcl_cout << (*ec1)[j] << "\n";
  vdgl_interpolator_sptr intp = new vdgl_interpolator_linear(ec1);
  vdgl_digital_curve_sptr dc1 = new vdgl_digital_curve(intp);
  pts.clear();
  good = bdgl_curve_algs::intersect_line(dc1, l, pts);
  if(good)
    {
      vcl_cout << "Num Intersection points = " << pts.size() << "\n";
      vcl_cout << " p[0] " << pts[0] << " p[1] " << pts[1] << "\n";
    }
  Assert(pts.size()==2);
  vcl_cout << "finished testing bdgl_curve_algs\n";
  vcl_cout << "Test Summary: " << success << " tests succeeded, "
           << failures << " tests failed" << (failures?"\t***\n":"\n");
  return failures;
}
