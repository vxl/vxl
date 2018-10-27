//:
// \file
#include <testlib/testlib_test.h>

#include "test_node_sptr.h"
#include "test_edge_sptr.h"
#include "test_graph_sptr.h"
#include "test_graph.h"

//: Test the graph class
//
// This is a test of using the templated graph class with
// regular pointers (as opposed to smart pointers)
static void graph_test()
{
  test_node_sptr v1 = new test_node(1);
  test_node_sptr v2 = new test_node(2);
  test_node_sptr v3 = new test_node(3);
  test_node_sptr v4 = new test_node(4);
  test_node_sptr v5 = new test_node(5);

  test_graph_sptr the_graph = new test_graph();

  TEST("Testing add_vertex()",
       the_graph->add_vertex(v1) &&
       the_graph->add_vertex(v2) &&
       the_graph->add_vertex(v3) &&
       the_graph->add_vertex(v4) &&
       the_graph->add_vertex(v5),
       true);

  TEST("Testing vertex existence",
       the_graph->exists(v1) &&
       the_graph->exists(v2) &&
       the_graph->exists(v3) &&
       the_graph->exists(v4) &&
       the_graph->exists(v5),
       true);

  TEST("Testing remove_vertex()",
       the_graph->remove_vertex(v3) &&
       the_graph->remove_vertex(v4),
       true);

  TEST("Testing vertex existence",
       !the_graph->exists(v3) &&
       !the_graph->exists(v4),
       true);

  // make the edges
  TEST("Testing add_edge()",
       the_graph->add_edge(v1, v2) &&
       the_graph->add_edge(v2, v1) &&
       the_graph->add_edge(v1, v3) && // v3 added back in
       the_graph->add_edge(v4, v2) && // v4 added back in
       the_graph->add_edge(v2, v4) &&
       the_graph->add_edge(v4, v3) &&
       the_graph->add_edge(v4, v5) &&
       the_graph->add_edge(v3, v5) &&
       the_graph->add_edge(v5, v3) &&
       !the_graph->add_edge(v5, v3) && // can't add the same edge twice
       !the_graph->add_edge(v1, v1), // can't edge to self
       true);

  TEST("Testing vertex degree",
       v4->degree()     == 4 &&
       v4->in_degree()  == 1 &&
       v4->out_degree() == 3,
       true);

  //testing cyclic_adj_succ function


  //testing cyclic_adj_pred function


  //remove nodes from the graph (should update connectivity)
  TEST("Testing remove_vertex() again",
       the_graph->remove_vertex(v3),
       true);

  // remove edges
  TEST("Testing remove_edge()",
       the_graph->remove_edge(v4, v5) &&
       !the_graph->remove_edge(v3, v5) && // can't remove an edge not in the graph
       !the_graph->remove_edge(v2, nullptr), // can't remove an invalid edge
       true);


  //testing iterators
  int num_edges=0;
  for (auto  e_itr = the_graph->edges_begin();
       e_itr != the_graph->edges_end(); ++e_itr)
  {
    num_edges++;
    //(*e_itr)->print_summary(std::cout);
  }

  TEST("Testing edge_iterator()",
       num_edges==4, //there should be 4 edges remaining in the graph
       true);

  int num_vertices=0;
  for (auto  v_itr = the_graph->vertices_begin();
       v_itr != the_graph->vertices_end(); ++v_itr)
  {
    num_vertices++;
    //(*v_itr)->print_summary(std::cout);
  }

  TEST("Testing vertex_iterator()",
       num_vertices==4, //there should be 4 nodes remaining in the graph
       true);

  //test graph clear function
  the_graph->clear();

  TEST("Testing clear()",
       the_graph->number_of_edges()==0 &&  //there should be no edges remaining
       the_graph->number_of_vertices()==0, //there should be no nodes remaining
       true);
}

TESTMAIN(graph_test);
