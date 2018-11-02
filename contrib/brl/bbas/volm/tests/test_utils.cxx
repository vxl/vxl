#include <iostream>
#include <testlib/testlib_test.h>
#include <volm/volm_utils.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void test_poly_contain_box()
{
  // create input box and polygon

  vgl_polygon<double> poly1;
  poly1.new_sheet();
  poly1.push_back(1.0, 2.0);   poly1.push_back(1.0, -1.0);  poly1.push_back(7.0, -1.0);  poly1.push_back(11.0, 9.0);
  poly1.push_back(8.0, 13.0);  poly1.push_back(5.0, 11.0);  poly1.push_back(3.0, 14.0);  poly1.push_back(-2.0, 14.0);
  poly1.push_back(-2.0, 8.0);
  poly1.push_back(-2.0, 6.0);  poly1.push_back(-3.0, 4.0);
  std::cout << "input polygon1:\n";
  poly1.print(std::cout);

  vgl_box_2d<double> box1(2.0, 4.0, 6.0, 8.0);
  std::cout << "input box1: " << box1 << std::endl;
  TEST("box1 is contained inside polygon1", volm_utils::poly_contains(poly1[0], box1), true);

  vgl_box_2d<double> box2(0.0, 10.0, 0.0, 12.0);
  std::cout << "input box2: " << box2 << std::endl;
  TEST("box2 is not contained inside polygon1", volm_utils::poly_contains(poly1[0], box2), false);

  vgl_polygon<float> poly2;
  poly2.new_sheet();
  poly2.push_back(12.0f, 4.0f);  poly2.push_back(14.0f, 4.0f);  poly2.push_back(14.0f, 6.0f);  poly2.push_back(12.0f, 6.0f);
  std::cout << "input polygon2:\n";
  poly2.print(std::cout);
  vgl_box_2d<float> box3(12.0f, 14.0f, 4.0f, 6.0f);
  std::cout << "input box3: " << box3 << std::endl;
  TEST("box3 is contained inside polygon2", volm_utils::poly_contains(poly2, box3), true);
  vgl_box_2d<float> box4(5.0f, 8.0f, 6.0f, 10.0f);
  std::cout << "input box4: " << box4 << std::endl;
  TEST("box4 is not contained inside polygon2", volm_utils::poly_contains(poly2,box4), false);
  return;
}

template <class T>
static void test_poly_contain_poly(std::string const& type_name)
{
  vgl_polygon<T> p_out;
  p_out.new_sheet();
  p_out.push_back((T)1.0,  (T)2.0);   p_out.push_back((T)1.0, (T)-1.0);  p_out.push_back((T)7.0, (T)-1.0);  p_out.push_back((T)11.0, (T)9.0);
  p_out.push_back((T)8.0,  (T)13.0);  p_out.push_back((T)5.0, (T)11.0);  p_out.push_back((T)3.0, (T)14.0);  p_out.push_back((T)-2.0, (T)14.0);
  p_out.push_back((T)-2.0, (T)8.0);   p_out.push_back((T)-2.0, (T)6.0);  p_out.push_back((T)-3.0, (T)4.0);
  std::cout << "outer <" << type_name << "> polygon:\n";
  p_out.print(std::cout);

  vgl_polygon<T> p_in;
  p_in.new_sheet();
  p_in.push_back((T)0.0, (T)6.0);    p_in.push_back((T)2.0, (T)4.0);  p_in.push_back((T)3.0, (T)8.0);  p_in.push_back((T)1.0, (T)10.0);
  p_in.new_sheet();
  p_in.push_back((T)4.0, (T)0.0);    p_in.push_back((T)8.0, (T)2.0);  p_in.push_back((T)6.0, (T)4.0);
  p_in.new_sheet();
  p_in.push_back((T)-4.0, (T)10.0);  p_in.push_back((T)0.0, (T)10.0); p_in.push_back((T)2.0, (T)14.0); p_in.push_back((T)-4.0, (T)14.0);
  p_in.new_sheet();
  p_in.push_back((T)10.0, (T)12.0);  p_in.push_back((T)12.0,(T)12.0); p_in.push_back((T)12.0,(T)15.0); p_in.push_back((T)10.0, (T)14.0);
  std::cout << "input inner <" << type_name << "> polygon:\n";
  p_in.print(std::cout);

  vgl_polygon<T> inner = volm_utils::poly_contains(p_out, p_in);
  std::cout << "output inner <" << type_name << "> polygon\n";
  inner.print(std::cout);
  TEST( ("input inner <" + type_name +  "> polygon has 2 sheets entirely inside outer <" + type_name + "> polygon").c_str(), inner.num_sheets(), 2);
  return;
}

static void test_utils()
{
  std::cout << "\n************************\n";
  std::cout << " Testing volm_util: polygon contains 2d box\n";
  std::cout << "************************\n\n";
  test_poly_contain_box();

  std::cout << "\n************************\n";
  std::cout << " Testing volm_util: polygon contains polygon\n";
  std::cout << "************************\n\n";
  test_poly_contain_poly<double>("double");
  test_poly_contain_poly<float>("float");

  return;
}

TESTMAIN(test_utils);
