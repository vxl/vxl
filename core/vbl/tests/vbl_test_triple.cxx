#include <testlib/testlib_test.h>
#include <vbl/vbl_triple.h>
#include <vcl_iostream.h>
#include <vcl_sstream.h>

static
void vbl_test_triple()
{
  vcl_cout << "\n\n\n"
           << "*****************\n"
           << " Test vbl_triple\n"
           << "*****************\n";
  vbl_triple<double,int,int> t(7.0,1,2);
  TEST("vbl_triple constructor", t.first == 7.0 && t.second == 1 && t.third == 2, true);
  vbl_triple<double,int,int> t2 = t; t2.first = 6.0;
  TEST("vbl_triple copy constructor", t2.first == 6.0 && t2.second == 1 && t2.third == 2, true);
  TEST("vbl_triple compare", t2 != t, true);
  TEST("vbl_triple compare", t2 < t, true);
  t2 = t; t2.third = 3;
  TEST("vbl_triple assignment operator", t2.first == 7.0 && t2.second == 1 && t2.third == 3, true);
  TEST("vbl_triple compare", t != t2, true);
  TEST("vbl_triple compare", t < t2, true);
  
  vcl_stringstream s;
  s << t;
  vbl_triple<double,int,int> t3;
  s >> t3;
  TEST("operator << and operator >>", t, t3);
}

TESTMAIN(vbl_test_triple);
