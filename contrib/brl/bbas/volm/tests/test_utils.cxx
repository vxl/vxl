#include <testlib/testlib_test.h>
#include <volm/volm_utils.h>
#include <vcl_iostream.h>

static void test_poly_contain_box()
{
  // create input box and polygon

  vgl_polygon<double> poly1;
  poly1.new_sheet();
  poly1.push_back(1.0, 2.0);   poly1.push_back(1.0, -1.0);  poly1.push_back(7.0, -1.0);  poly1.push_back(11.0, 9.0);
  poly1.push_back(8.0, 13.0);  poly1.push_back(5.0, 11.0);  poly1.push_back(3.0, 14.0);  poly1.push_back(-2.0, 14.0);
  poly1.push_back(-2.0, 8.0);
  poly1.push_back(-2.0, 6.0);  poly1.push_back(-3.0, 4.0);
  vcl_cout << "input polygon1:\n";
  poly1.print(vcl_cout);

  vgl_box_2d<double> box1(2.0, 4.0, 6.0, 8.0);
  vcl_cout << "input box1: " << box1 << vcl_endl;
  TEST("box1 is contained inside polygon1", volm_utils::poly_contains(poly1[0], box1), true);

  vgl_box_2d<double> box2(0.0, 10.0, 0.0, 12.0);
  vcl_cout << "input box2: " << box2 << vcl_endl;
  TEST("box2 is not contained inside polygon1", volm_utils::poly_contains(poly1[0], box2), false);

  vgl_polygon<float> poly2;
  poly2.new_sheet();
  poly2.push_back(12.0f, 4.0f);  poly2.push_back(14.0f, 4.0f);  poly2.push_back(14.0f, 6.0f);  poly2.push_back(12.0f,
                                                                                                               6.0f);
  vcl_cout << "input polygon2:\n";
  poly2.print(vcl_cout);
  vgl_box_2d<float> box3(12.0f, 14.0f, 4.0f, 6.0f);
  vcl_cout << "input box3: " << box3 << vcl_endl;
  TEST("box3 is contained inside polygon2", volm_utils::poly_contains(poly2, box3), true);
  vgl_box_2d<float> box4(5.0f, 8.0f, 6.0f, 10.0f);
  vcl_cout << "input box4: " << box4 << vcl_endl;
  TEST("box4 is not contained inside polygon2", volm_utils::poly_contains(poly2, box4), false);
  return;
}

template <class T>
static void test_poly_contain_poly(T type, vcl_string const& type_name)
{
  vgl_polygon<T> p_out;
  p_out.new_sheet();
  p_out.push_back( (T)1.0,  (T)2.0);   p_out.push_back( (T)1.0, (T)-1.0);  p_out.push_back( (T)7.0, (T)-1.0);
  p_out.push_back( (T)11.0, (T)9.0);
  p_out.push_back( (T)8.0,  (T)13.0);  p_out.push_back( (T)5.0, (T)11.0);  p_out.push_back( (T)3.0, (T)14.0);
  p_out.push_back( (T)-2.0, (T)14.0);
  p_out.push_back( (T)-2.0, (T)8.0);   p_out.push_back( (T)-2.0, (T)6.0);  p_out.push_back( (T)-3.0, (T)4.0);
  vcl_cout << "outer <" << type_name << "> polygon:\n";
  p_out.print(vcl_cout);

  vgl_polygon<T> p_in;
  p_in.new_sheet();
  p_in.push_back( (T)0.0, (T)6.0);    p_in.push_back( (T)2.0, (T)4.0);  p_in.push_back( (T)3.0, (T)8.0);
  p_in.push_back( (T)1.0, (T)10.0);
  p_in.new_sheet();
  p_in.push_back( (T)4.0, (T)0.0);    p_in.push_back( (T)8.0, (T)2.0);  p_in.push_back( (T)6.0, (T)4.0);
  p_in.new_sheet();
  p_in.push_back( (T)-4.0, (T)10.0);  p_in.push_back( (T)0.0, (T)10.0); p_in.push_back( (T)2.0, (T)14.0);
  p_in.push_back( (T)-4.0, (T)14.0);
  p_in.new_sheet();
  p_in.push_back( (T)10.0, (T)12.0);  p_in.push_back( (T)12.0, (T)12.0); p_in.push_back( (T)12.0, (T)15.0);
  p_in.push_back( (T)10.0, (T)14.0);
  vcl_cout << "input inner <" << type_name << "> polygon:\n";
  p_in.print(vcl_cout);

  vgl_polygon<T> inner = volm_utils::poly_contains(p_out, p_in);
  vcl_cout << "output inner <" << type_name << "> polygon\n";
  inner.print(vcl_cout);
  TEST(
    ("input inner <" + type_name +  "> polygon has 2 sheets entirely inside outer <" + type_name + "> polygon").c_str(),
    inner.num_sheets(), 2);
  return;
}

static void test_utils()
{
  vcl_cout << "\n************************\n";
  vcl_cout << " Testing volm_util: polygon contains 2d box\n";
  vcl_cout << "************************\n\n";
  test_poly_contain_box();

  vcl_cout << "\n************************\n";
  vcl_cout << " Testing volm_util: polygon contains polygon\n";
  vcl_cout << "************************\n\n";
  test_poly_contain_poly(0.1, "double");
  test_poly_contain_poly(0.1f, "float");

  return;
}

TESTMAIN(test_utils);
