#include <iostream>
#include <testlib/testlib_test.h>
#include <vbl/vbl_disjoint_sets.h>
#include <vbl/vbl_edge.h>
#include <vbl/vbl_graph_partition.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

static void print_element_sets(const vbl_disjoint_sets & s)
{
  for (int i = 0; i < s.num_elements(); ++i){
    int k = s.find_set(i);
    std::cout << "[" << k << "]: " << s.size(k) << "  ";
  }
  std::cout << std::endl;
}

static void vbl_test_disjoint_sets()
{
  vbl_disjoint_sets s(10);
  s.set_union(s.find_set(5),s.find_set(3));
  s.set_union(s.find_set(1),s.find_set(3));
  s.set_union(s.find_set(6),s.find_set(7));
  s.set_union(s.find_set(8),s.find_set(9));
  s.set_union(s.find_set(6),s.find_set(9));
  s.add_elements(3);
  s.set_union(s.find_set(11),s.find_set(12));
  s.set_union(s.find_set(9),s.find_set(10));
  s.set_union(s.find_set(7),s.find_set(11));
  print_element_sets(s);
  int n0 = s.size(0), n2 = s.size(2),n4 = s.size(4),
    n5 = s.size(5), n6 = s.size(6);
  bool good = n0==1&&n2==1&&n4==1&&n5==3&&n6==7;
  good = good && s.num_sets() == 5;
  TEST("disjoint set operations", good, true);

  // test partitioning a graph
  int n_verts = 10;
  std::vector<vbl_edge> edges;
  for(int i = 1; i<n_verts; ++i)
    edges.emplace_back(i-1, i, 0.0f);

  edges[(n_verts/2)-1].w_ = 5.0f;
  edges[n_verts/5].w_ = 5.0f;
 vbl_disjoint_sets ds(n_verts);
  vbl_graph_partition(ds, edges, 10);
  print_element_sets(ds);
  int gn0 = ds.size(0), gn3 = ds.size(3),gn5 = ds.size(5);
  good = gn0==3&&gn3==2&&gn5==5;
  TEST("graph partition", good, true);
}

TESTMAIN(vbl_test_disjoint_sets);
