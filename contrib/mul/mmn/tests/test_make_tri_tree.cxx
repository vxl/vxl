// This is mul/mmn/tests/test_make_tri_tree.cxx
#include <iostream>
#include <vector>
#include <testlib/testlib_test.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <mmn/mmn_make_tri_tree.h>


void test_make_tri_tree_a()
{
  std::cout<<"==== test mmn_make_tri_tree (triangle) ====="<<std::endl;

  std::vector<mmn_arc> arcs;

  std::cout<<"Test case where all distances equal."<<std::endl;
  vnl_matrix<double> D(3,3);
  D.fill(1.0);
  for (unsigned i=0;i<3;++i) D(i,i)=0.0;

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  std::cout<<arcs<<std::endl;

  std::cout<<"Test case where all distances zero."<<std::endl;
  D.fill(0.0);

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  std::cout<<arcs<<std::endl;

  std::cout<<"Test case where all distances huge."<<std::endl;
  D.fill(9.9e19);

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  std::cout<<arcs<<std::endl;
}

void test_make_tri_tree_b()
{
  std::cout<<"==== test mmn_make_tri_tree (line) ====="<<std::endl;
  int n=4;
  vnl_matrix<double> D(n,n);
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j) D(i,j)=(i-j)*(i-j);

  std::vector<mmn_arc> arcs;

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),(unsigned int)(n*2-3));

  std::cout<<arcs<<std::endl;
  TEST("Arc 0",arcs[0],mmn_arc(0,1));
}

void test_make_tri_tree()
{
  test_make_tri_tree_a();
  test_make_tri_tree_b();
}

TESTMAIN(test_make_tri_tree);
