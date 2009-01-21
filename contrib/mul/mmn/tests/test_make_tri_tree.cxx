// This is mul/mmn/tests/test_make_tri_tree.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <mmn/mmn_make_tri_tree.h>


void test_make_tri_tree_a()
{
  vcl_cout<<"==== test mmn_make_tri_tree (triangle) ====="<<vcl_endl;

  vcl_vector<mmn_arc> arcs;

  vcl_cout<<"Test case where all distances equal."<<vcl_endl;
  vnl_matrix<double> D(3,3);
  D.fill(1.0);
  for (unsigned i=0;i<3;++i) D(i,i)=0.0;

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  vcl_cout<<arcs<<vcl_endl;

  vcl_cout<<"Test case where all distances zero."<<vcl_endl;
  D.fill(0.0);

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  vcl_cout<<arcs<<vcl_endl;

  vcl_cout<<"Test case where all distances huge."<<vcl_endl;
  D.fill(9.9e19);

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),3);

  vcl_cout<<arcs<<vcl_endl;
}

void test_make_tri_tree_b()
{
  vcl_cout<<"==== test mmn_make_tri_tree (line) ====="<<vcl_endl;
  int n=4;
  vnl_matrix<double> D(n,n);
  for (int i=0;i<n;++i)
    for (int j=0;j<n;++j) D(i,j)=(i-j)*(i-j);

  vcl_vector<mmn_arc> arcs;

  mmn_make_tri_tree(D,arcs);
  TEST("N.arcs",arcs.size(),(unsigned int)(n*2-3));

  vcl_cout<<arcs<<vcl_endl;
  TEST("Arc 0",arcs[0],mmn_arc(0,1));
}

void test_make_tri_tree()
{
  test_make_tri_tree_a();
  test_make_tri_tree_b();
}

TESTMAIN(test_make_tri_tree);
