// This is brl/bbas/bdgl/tests/test_peano_curve.cxx
#include <iostream>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <bdgl/bdgl_peano_curve.h>
#include <testlib/testlib_test.h>

static void test_peano_curve()
{
  std::vector<vgl_point_2d<double> > gt;
  gt.emplace_back(-0.75,-0.75);
  gt.emplace_back(-0.25,-0.75);
  gt.emplace_back(-0.25,-0.25);
  gt.emplace_back(-0.75,-0.25);
  gt.emplace_back(-0.75,0.25);
  gt.emplace_back(-0.75,0.75);
  gt.emplace_back(-0.25,0.75);
  gt.emplace_back(-0.25,0.25);
  gt.emplace_back(0.25,0.25);
  gt.emplace_back(0.25,0.75);
  gt.emplace_back(0.75,0.75);
  gt.emplace_back(0.75,0.25);
  gt.emplace_back(0.75,-0.25);
  gt.emplace_back(0.25,-0.25);
  gt.emplace_back(0.25,-0.75);
  gt.emplace_back(0.75,-0.75);

  std::vector<vgl_point_2d<double> > p=recurse_peano_curve(2);

  bool flag=true;
  for (unsigned i=0;i<p.size();i++)
  {
    if (p[i]!=gt[i])
      flag=false;
  }
  TEST("Peano Curve for level 2", flag, true);
}

TESTMAIN(test_peano_curve);
