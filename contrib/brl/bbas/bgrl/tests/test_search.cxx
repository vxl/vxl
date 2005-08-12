#include <testlib/testlib_test.h>

#include <bgrl/bgrl_search_func.h>
#include <bgrl/bgrl_vertex.h>
#include <bgrl/bgrl_graph.h>

bool test_iterator( bgrl_graph::iterator& itr,
                    bgrl_vertex_sptr truth[] )
{
  bool check = true;
  int i=0;
  bgrl_graph_sptr the_graph = itr.graph();
  int size = the_graph->size();
  for (; itr != the_graph->end(); ++itr, ++i)
  {
    if (i >= size) {
      check = false; ++i;
      break;
    }
    if ((*itr) != truth[i]) {
      vcl_cout << "Incorrect value for i=" << i << '\n';
      check = false;
    }
  }
  if (i < size)
    check = false;

  vcl_cout << "size = " << size << ", imax = " << i << '\n';
  return check;
}

// Test the iterators
void test_search()
{
  // create vertices
  bgrl_vertex_sptr vertex_1 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_2 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_3 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_4 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_5 = new bgrl_vertex();

  // create the graph
  bgrl_graph_sptr the_graph = new bgrl_graph;

  // add the vertices
  the_graph->add_vertex(vertex_1);
  the_graph->add_vertex(vertex_2);
  the_graph->add_vertex(vertex_3);
  the_graph->add_vertex(vertex_4);
  the_graph->add_vertex(vertex_5);

  // add the edges
  the_graph->add_edge(vertex_1, vertex_2);
  the_graph->add_edge(vertex_2, vertex_1);
  the_graph->add_edge(vertex_1, vertex_3);
  the_graph->add_edge(vertex_3, vertex_1);
  the_graph->add_edge(vertex_4, vertex_2);
  the_graph->add_edge(vertex_2, vertex_4);
  the_graph->add_edge(vertex_4, vertex_3);
  the_graph->add_edge(vertex_4, vertex_5);
  the_graph->add_edge(vertex_3, vertex_5);
  the_graph->add_edge(vertex_5, vertex_3);

  // These tests could use some improvement.  There are multiple acceptable
  // results, thoses used below will be correct if the vertices and edges defined
  // above have monotonically increasing memory addresses.
  bgrl_vertex_sptr default_order[] = {vertex_1, vertex_2, vertex_3, vertex_4, vertex_5};
  bgrl_graph::iterator default_itr = the_graph->begin();
  TEST("Testing default iterator", test_iterator(default_itr, default_order ), true);

  bgrl_vertex_sptr depth_order[] = {vertex_4, vertex_5, vertex_3, vertex_1, vertex_2};
  bgrl_graph::iterator depth_itr = the_graph->begin(new bgrl_depth_search(vertex_4));
  TEST("Testing depth_iterator",  test_iterator(depth_itr, depth_order ), true);

  bgrl_vertex_sptr breadth_order[] = {vertex_4, vertex_2, vertex_3, vertex_5, vertex_1};
  bgrl_graph::iterator breadth_itr = the_graph->begin(new bgrl_breadth_search(vertex_4));
  TEST("Testing breadth_iterator", test_iterator(breadth_itr, breadth_order ), true);
}

MAIN( test_search )
{
  START( "bgrl_search" );

  test_search();

  SUMMARY();
}
