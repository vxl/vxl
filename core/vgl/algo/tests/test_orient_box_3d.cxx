// Test vgl_orient_box_3d
// JLM

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_double_3.h>
#include <vnl/vnl_math.h> // for pi
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
  vnl_quaternion<double> q(v, 0.25*vnl_math::pi);
  vgl_orient_box_3d<double> obox(abox, q);
  std::cout << "oriented box\n " << obox << '\n'
           << "Corners\n";
  std::vector<vgl_point_3d<double> > c = obox.corners();
  for (auto & pit : c)
    std::cout << pit << '\n';
  vgl_point_3d<double> tp(0.5, 0.4, -0.1);
  bool inside = obox.contains(tp);
  bool outside = !(obox.contains(1.0, 1.0, 1.0));
  TEST("test oriented box 3d contains predicate",inside&&outside, true);
}

static void test_construct()
{
  // construct a bounding box from 4 corner points (orthogonal sides, and oriented!):
  vgl_point_3d<double> p0(1,2,3),
                       px(1,6,0),
                       py(1,5,7),
                       pz(8,2,3);
  vgl_orient_box_3d<double> obox(p0,px,py,pz);
  TEST_NEAR("width",  obox.width(),  5, 1e-12);
  TEST_NEAR("height", obox.height(), 5, 1e-12);
  TEST_NEAR("depth",  obox.depth(),  7, 1e-12);

  std::vector<vgl_point_3d<double> > c = obox.corners();
  auto pit = c.begin();
  TEST_NEAR("corner 1", (*pit - vgl_point_3d<double>(1,2,3)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 2", (*pit - vgl_point_3d<double>(1,6,0)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 3", (*pit - vgl_point_3d<double>(8,2,3)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 4", (*pit - vgl_point_3d<double>(8,6,0)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 5", (*pit - vgl_point_3d<double>(1,5,7)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 6", (*pit - vgl_point_3d<double>(1,9,4)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 7", (*pit - vgl_point_3d<double>(8,5,7)).length(), 0.0, 1e-12);
  ++pit;
  TEST_NEAR("corner 8", (*pit - vgl_point_3d<double>(8,9,4)).length(), 0.0, 1e-12);
}

void test_orient_box_3d()
{
  std::cout << "***************************\n"
           << " Testing vgl_orient_box_3d\n"
           << "***************************\n\n";

  test_contains();
  test_construct();
}

TESTMAIN(test_orient_box_3d);
