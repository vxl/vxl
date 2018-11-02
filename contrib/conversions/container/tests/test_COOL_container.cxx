#include <iostream>
#include <testlib/testlib_test.h>

#ifdef HAS_NO_COOL

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
void test_COOL_container()
{
  std::cerr << "test not run since you do not have the TargetJr COOL package\n";
}

#else
#include <container/std::vector_to_CoolArray.h>
#include <container/CoolArray_to_CoolList.h>
#include <container/CoolList_to_vcl_list.h>
#include <container/std::list_to_CoolArray.h>
#include <container/CoolArray_to_vcl_vector.h>
#include <container/std::vector_to_CoolList.h>
#include <container/CoolList_to_CoolArray.h>
#include <container/CoolArray_to_vcl_list.h>
#include <container/std::list_to_CoolList.h>
#include <container/CoolList_to_vcl_vector.h>

void test_COOL_container()
{
  int data[] = { 1, 2, 3, 2, 1, 0, -1 };
  std::vector<int> l(data, data+7);
  CoolArray<int>  a = std::vector_to_CoolArray(l);
  TEST("container conversions", a[0]==1 && a[1]==2 && a[2]==3 && a[3]==2 && a[4]==1 && a[5]==0 && a[6]==-1, true);
  CoolList<int>   b = CoolArray_to_CoolList(a);
  TEST("container conversions", b[0]==1 && b[1]==2 && b[2]==3 && b[3]==2 && b[4]==1 && b[5]==0 && b[6]==-1, true);
  std::list<int>   c = CoolList_to_vcl_list(b);
  TEST("container conversions", *(c.begin()), 1);
  CoolArray<int>  d = std::list_to_CoolArray(c);
  TEST("container conversions", d[0]==1 && d[1]==2 && d[2]==3 && d[3]==2 && d[4]==1 && d[5]==0 && d[6]==-1, true);
  std::vector<int> e = CoolArray_to_vcl_vector(d);
  TEST("container conversions", e[0]==1 && e[1]==2 && e[2]==3 && e[3]==2 && e[4]==1 && e[5]==0 && e[6]==-1, true);
  CoolList<int>   f = std::vector_to_CoolList(e);
  TEST("container conversions", f[0]==1 && f[1]==2 && f[2]==3 && f[3]==2 && f[4]==1 && f[5]==0 && f[6]==-1, true);
  CoolArray<int>  g = CoolList_to_CoolArray(f);
  TEST("container conversions", g[0]==1 && g[1]==2 && g[2]==3 && g[3]==2 && g[4]==1 && g[5]==0 && g[6]==-1, true);
  std::list<int>   h = CoolArray_to_vcl_list(g);
  TEST("container conversions", *(h.begin()), 1);
  CoolList<int>   i = std::list_to_CoolList(h);
  TEST("container conversions", i[0]==1 && i[1]==2 && i[2]==3 && i[3]==2 && i[4]==1 && i[5]==0 && i[6]==-1, true);
  std::vector<int> j = CoolList_to_vcl_vector(i);
  TEST("container conversions", j[0]==1 && j[1]==2 && j[2]==3 && j[3]==2 && j[4]==1 && j[5]==0 && j[6]==-1, true);
}

#endif // HAS_NO_COOL

TESTMAIN(test_COOL_container);
