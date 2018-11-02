#include <iostream>
#include <testlib/testlib_test.h>
#include <vbl/vbl_quadruple.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static
void vbl_test_quadruple()
{
  std::cout << "\n\n\n"
           << "********************\n"
           << " Test vbl_quadruple\n"
           << "********************\n";
  vbl_quadruple<int,int,int,int> t(7,0,1,2);
  TEST("vbl_quadruple constructor", t.first == 7 && t.second == 0 && t.third == 1 && t.fourth == 2, true);
  vbl_quadruple<int,int,int,int> t2 = t; t2.first = 6;
  TEST("vbl_quadruple copy constructor", t2.first == 6 && t2.second == 0 && t2.third == 1 && t2.fourth == 2, true);
  TEST("vbl_quadruple compare", t2 != t, true);
  TEST("vbl_quadruple compare", t2 < t, true);
  t2 = t; t2.third = 3;
  TEST("vbl_quadruple assignment operator", t2.first == 7 && t2.second == 0 && t2.third == 3 && t2.fourth == 2, true);
  TEST("vbl_quadruple compare", t != t2, true);
  TEST("vbl_quadruple compare", t < t2, true);
}

TESTMAIN(vbl_test_quadruple);
