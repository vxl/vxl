// This is mul/vil3d/tests/test_algo_structuring_element.cxx
#include <testlib/testlib_test.h>
#include <vcl_iostream.h>
#include <vil3d/algo/vil3d_structuring_element.h>

static void test_algo_structuring_element()
{
  vcl_cout << "****************************\n"
           << " Testing vil3d_structuring_element\n"
           << "****************************\n";

  int n = 5;
  vcl_vector<int> px(n),py(n),pz(n);
  for (int i=0;i<n;++i)
  {
    px[i]=i; py[i]=i+3; pz[i]=i-2;
  }

  vil3d_structuring_element se(px,py,pz);

  vcl_cout<<"Element: "<<se<<vcl_endl;

  TEST("min_i()",se.min_i(),0);
  TEST("max_i()",se.max_i(),n-1);
  TEST("min_j()",se.min_j(),3);
  TEST("max_j()",se.max_j(),n+2);
  TEST("min_k()",se.min_k(),-2);
  TEST("max_k()",se.max_k(),n-3);

  se.set_to_sphere(1.1);
  TEST("Number of elements in sphere",se.p_i().size(),7);

  vcl_cout<<"set_to_line_i():\n";
  se.set_to_line_i(-2,7);
  TEST("Number of elements",se.p_i().size(),10);
  TEST("min_i()",se.min_i(),-2);
  TEST("max_i()",se.max_i(),7);
  TEST("min_j()",se.min_j(),0);
  TEST("max_j()",se.max_j(),0);
  TEST("min_k()",se.min_k(),0);
  TEST("max_k()",se.max_k(),0);

  vcl_cout<<"set_to_line_j():\n";
  se.set_to_line_j(-1,6);
  TEST("Number of elements",se.p_i().size(),8);
  TEST("min_i()",se.min_i(),0);
  TEST("max_i()",se.max_i(),0);
  TEST("min_j()",se.min_j(),-1);
  TEST("max_j()",se.max_j(),6);
  TEST("min_k()",se.min_k(),0);
  TEST("max_k()",se.max_k(),0);

  vcl_cout<<"set_to_line_k():\n";
  se.set_to_line_k(-3,8);
  TEST("Number of elements",se.p_i().size(),12);
  TEST("min_i()",se.min_i(),0);
  TEST("max_i()",se.max_i(),0);
  TEST("min_j()",se.min_j(),0);
  TEST("max_j()",se.max_j(),0);
  TEST("min_k()",se.min_k(),-3);
  TEST("max_k()",se.max_k(),8);
}

TESTMAIN(test_algo_structuring_element);
