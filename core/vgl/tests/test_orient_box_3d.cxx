// Test vgl_orient_box_3d
// JLM

#include <vcl_iostream.h>
#include <vcl_vector.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_double_3.h>
#include <testlib/testlib_test.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/algo/vgl_orient_box_3d.h>


static void test_contains()
{
  vgl_point_3d<double> p1(0,0,0);
  vgl_point_3d<double> p2(1,1,1);
  vgl_box_3d<double> abox;
  abox.add(p1); abox.add(p2);
  vnl_double_3 v(1.0, 0,0);
  vnl_quaternion<double> q(v, 3.14159/4);
  vgl_orient_box_3d<double> obox(abox, q);
  vcl_cout << "oriented box\n " << obox << '\n';
  vcl_cout << "Corners\n ";
  vcl_vector<vgl_point_3d<double> > c = obox.corners();
  for(vcl_vector<vgl_point_3d<double> >::iterator pit = c.begin();
      pit != c.end(); ++pit)
    vcl_cout << *pit << '\n';
  vgl_point_3d<double> tp(0.5, 0.4, -0.1);
  bool inside = obox.contains(tp);
  bool outside = !(obox.contains(1.0, 1.0, 1.0));
  TEST("test oriented box 3d contains predicate",inside&&outside, true);
}





void test_orient_box_3d()
{
  vcl_cout << "*****************************\n"
           << " Testing vgl_orient_box_3d\n"
           << "*****************************\n\n";

  test_contains();

}


TESTMAIN(test_orient_box_3d);
