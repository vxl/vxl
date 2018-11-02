#include <string>
#include <iostream>
#include <sstream>
#include <testlib/testlib_test.h>
#include <bsta/bsta_gaussian_full.h>
#include <bsta/algo/bsta_display_vrml.h>
#include <vgl/algo/vgl_rotation_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
static void test_display_vrml()
{
  vnl_vector_fixed<float, 3> mean(0.0f, 0.0f, 0.0f);
  vnl_matrix_fixed<float, 3, 3> covar(0.0f), dcovar(0.0f);
  covar[0][0] = 2.0f;  covar[0][1] = 1.0f; covar[0][2]=1.0f;
  covar[1][0] = 1.0f;  covar[1][1] = 1.625f; covar[1][2]=1.375f;
  covar[2][0] = 1.0f;  covar[2][1] = 1.375f; covar[2][2]=1.625f;
  std::cout << "Original Covar \n" << covar << '\n';
  bsta_gaussian_full<float, 3> g(mean, covar);
  std::stringstream ss;
  bool good = bsta_display_vrml(ss, g);
  std::string str = ss.str();
#ifdef VRML_DEBUG
  std::cout << str << '\n';
#endif
  std::string test("0.541766 0.0713248 0.837498 4.28395");
  unsigned pos = str.find(test);
  good = good && pos == 84;
  TEST("gaussian full display", good, true);
  dcovar[0][0] = 5.0f;  dcovar[1][1] = 3.0f; dcovar[2][2]=1.0f;
  vnl_vector_fixed<float, 3> mean2(5.0f, 0.0f, 0.0f);
  bsta_gaussian_full<float, 3> g2(mean2, dcovar);
  bsta_mixture< bsta_gaussian_full<float, 3> > mix;
  good = mix.insert(g, 0.5f);
  good = good && mix.insert(g2, 0.5f);
  std::stringstream ss2;
  good = good && bsta_display_vrml(ss2, mix);
  std::string str2 = ss2.str();
#ifdef VRML_DEBUG
  std::cout << str2 << '\n';
#endif
  std::string test2("2.23607 1.73205 1");
  pos = str2.find(test2);
  good = good && pos == 491;
  TEST("gaussian full mixture display", good, true);

}

TESTMAIN(test_display_vrml);
