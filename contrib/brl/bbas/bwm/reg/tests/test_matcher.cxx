// This is brl/bbas/bwm/reg/tests/test_matcher.cxx
#include "testlib/testlib_test.h"
#include <bwm/reg/bwm_reg_matcher.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>

void test_matcher()
{
  // define two digital curves
  vsol_point_2d_sptr p00 = new vsol_point_2d(3,3);
  vsol_point_2d_sptr p01 = new vsol_point_2d(6,3);
  vsol_point_2d_sptr p02 = new vsol_point_2d(6,6);
  std::vector<vsol_point_2d_sptr> verts0;
  verts0.push_back(p00);   verts0.push_back(p01);
  verts0.push_back(p02);
  vsol_digital_curve_2d_sptr modelc = new vsol_digital_curve_2d(verts0);
  std::vector<vsol_digital_curve_2d_sptr> model;
  model.push_back(modelc);
  vsol_point_2d_sptr p10 = new vsol_point_2d(5,5);
  vsol_point_2d_sptr p11 = new vsol_point_2d(8,5);
  vsol_point_2d_sptr p12 = new vsol_point_2d(8,8);
  std::vector<vsol_point_2d_sptr> verts1;
  verts1.push_back(p10);   verts1.push_back(p11);
  verts1.push_back(p12);
  vsol_digital_curve_2d_sptr searchc = new vsol_digital_curve_2d(verts1);
  std::vector<vsol_digital_curve_2d_sptr> search;
 search.push_back(searchc);

  bwm_reg_matcher matcher(model, 0, 0, 12, 12, search);

  int mcol = 0, mrow = 0;
  double distance_threshold = 0.0, angle_tol = 0.01, min_probability = 0.8;
  /*bool success=*/ matcher.match(mcol, mrow, distance_threshold, angle_tol, min_probability);
  TEST("Match test ", mcol, 1);
}

TESTMAIN(test_matcher);
