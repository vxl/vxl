// This is brl/bbas/bwm/reg/tests/test_matcher.cxx
#include <vcl_iostream.h>
#include <testlib/testlib_test.h>
#include <bwm/reg/bwm_reg_edge_champher.h>
#include <vsol/vsol_point_2d.h>
#include <vsol/vsol_digital_curve_2d_sptr.h>
void test_champher()
{

  // define two digital curves
  vsol_point_2d_sptr p00 = new vsol_point_2d(3,3);
  vsol_point_2d_sptr p01 = new vsol_point_2d(6,3);
  vsol_point_2d_sptr p02 = new vsol_point_2d(6,6);
  vcl_vector<vsol_point_2d_sptr> verts0;
  verts0.push_back(p00);   verts0.push_back(p01);
  verts0.push_back(p02);
  vsol_digital_curve_2d_sptr modelc = new vsol_digital_curve_2d(verts0);  
  vcl_vector<vsol_digital_curve_2d_sptr> model;
  model.push_back(modelc);
  bwm_reg_edge_champher ch(8, 8, model);
  vcl_cout.precision(3);
  for(unsigned r = 0; r<8; ++r)
    {
      for(unsigned c = 0; c<8; ++c)
        vcl_cout << ch.distance(c, r) << ' ';
      vcl_cout << '\n';
    }
  TEST("Test champher distance case1", ch.distance(0,0), 4);

  vsol_point_2d_sptr p10 = new vsol_point_2d(5,5);
  vsol_point_2d_sptr p11 = new vsol_point_2d(8,5);
  vsol_point_2d_sptr p12 = new vsol_point_2d(8,8);
  vcl_vector<vsol_point_2d_sptr> verts1;
  verts1.push_back(p10);   verts1.push_back(p11);
  verts1.push_back(p12);
  vsol_digital_curve_2d_sptr searchc = new vsol_digital_curve_2d(verts1);  
  vcl_vector<vsol_digital_curve_2d_sptr> search;
  search.push_back(searchc);
  bwm_reg_edge_champher ch1(10, 10, search);
  for(unsigned r = 0; r<10; ++r)
    {
      for(unsigned c = 0; c<10; ++c)
        vcl_cout << ch1.distance(c, r) << ' ';
      vcl_cout << '\n';
    }
TEST("Test champher distance case2", ch1.distance(0,0), 6);
}

TESTMAIN(test_champher);
