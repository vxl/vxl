// This is mul/fhs/tests/test_arc.cxx
#include <testlib/testlib_test.h>
#include <vcl_vector.h>
#include <fhs/fhs_arc.h>


void test_arc()
{
  vcl_cout<<"==== test fhs_arc ====="<<vcl_endl;

  unsigned n=4;
  // Generate linked list
  vcl_vector<fhs_arc> arc0(n);
  for (unsigned i=0;i<n;++i)
    arc0[i]=fhs_arc(i,i+1, 1+i,2*i, i*i, i*i);

  vcl_vector<fhs_arc> new_arc;
  vcl_vector<vcl_vector<unsigned> > children;

  // Arrange that node 2 is the root
  fhs_order_tree_from_root(arc0,new_arc,children,2);

  TEST("Correct number of arcs",new_arc.size(),n);
  TEST("Correct root",new_arc[0].i(),2);
  TEST("Root has two children",children[2].size(),2);
  TEST("Node 0 has no children",children[0].size(),0);
  TEST("Node n has no children",children[n].size(),0);
  for (unsigned i=0;i<n;++i)
    vcl_cout<<"Arc "<<i<<") "<<new_arc[i]<<vcl_endl;
}

TESTMAIN(test_arc);
