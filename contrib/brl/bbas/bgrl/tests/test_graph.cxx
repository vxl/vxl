//:
// \file
#include <testlib/testlib_test.h>
#include <vsl/vsl_binary_io.h>
#include <vbl/io/vbl_io_smart_ptr.h>
#include <vpl/vpl.h>

#include <bgrl/bgrl_vertex.h>
#include <bgrl/bgrl_edge.h>
#include <bgrl/bgrl_graph.h>

//: Test the graph class
void test_graph(const bgrl_vertex_sptr& vertex_1)
{
  bgrl_vertex_sptr vertex_2 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_3 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_4 = new bgrl_vertex();
  bgrl_vertex_sptr vertex_5 = new bgrl_vertex();

  bgrl_graph_sptr the_graph = new bgrl_graph;

  TEST("Testing add_vertex()",
       the_graph->add_vertex(vertex_1) &&
       the_graph->add_vertex(vertex_2) &&
       the_graph->add_vertex(vertex_3) &&
       the_graph->add_vertex(vertex_4) &&
       the_graph->add_vertex(vertex_5) &&
       !the_graph->add_vertex(vertex_5), // can't add a vertex twice
       true);
  
  TEST("Testing remove_vertex()",
       the_graph->remove_vertex(vertex_3) &&
       the_graph->remove_vertex(vertex_4) &&
       !the_graph->remove_vertex(vertex_4), // can't remove a vertex not in the graph
       true);

  // make the edges
  TEST("Testing add_edge()",     
       the_graph->add_edge(vertex_1, vertex_2) &&
       the_graph->add_edge(vertex_2, vertex_1) &&
       the_graph->add_edge(vertex_1, vertex_3) && // vertex_3 added back in
       the_graph->add_edge(vertex_4, vertex_2) && // vertex_4 added back in
       the_graph->add_edge(vertex_2, vertex_4) &&
       the_graph->add_edge(vertex_4, vertex_3) &&
       the_graph->add_edge(vertex_4, vertex_5) &&
       the_graph->add_edge(vertex_3, vertex_5) &&
       the_graph->add_edge(vertex_5, vertex_3) &&
       !the_graph->add_edge(vertex_5, vertex_3) && // can't add the same edge twice
       !the_graph->add_edge(vertex_1, vertex_1), // can't edge to self
       true);

  TEST("Testing vertex degree",
       vertex_4->degree()     == 4 &&
       vertex_4->in_degree()  == 1 &&
       vertex_4->out_degree() == 3,
       true);

  // remove edges
  TEST("Testing remove_edge()",
       the_graph->remove_edge(vertex_3, vertex_5) &&
       !the_graph->remove_edge(vertex_3, vertex_5) && // can't remove an edge not in the graph
       !the_graph->remove_edge(vertex_2, NULL), // can't remove an invalid edge
       true);

  TEST("Testing size()",
       the_graph->size() == 5,
       true);

  TEST("Testing purge()",the_graph->purge(),false);

// This test is needs to be updated
//
//  bgrl_vertex_sptr depth_order[] = {vertex_4, vertex_5, vertex_3, vertex_2, vertex_1};
//  bool depth_check = true;
//  bgrl_graph::iterator d_itr = the_graph->depth_begin(vertex_4);
//  for(int i=0; d_itr != the_graph->depth_end(); ++d_itr, ++i)
//    depth_check = ((*d_itr) == depth_order[i]) && depth_check;
//  TEST("Testing depth_iterator", depth_check, true);
//
//  bgrl_vertex_sptr breadth_order[] = {vertex_4, vertex_2, vertex_3, vertex_5, vertex_1};
//  bool breadth_check = true;
//  bgrl_graph::iterator b_itr = the_graph->breadth_begin(vertex_4);
//  for(int i=0; b_itr != the_graph->breadth_end(); ++b_itr, ++i)
//    breadth_check = ((*b_itr) == breadth_order[i]) && breadth_check;
//  TEST("Testing breadth_iterator", breadth_check, true);

//----------------------------------------------------------------------------------------
// I/O Tests
//----------------------------------------------------------------------------------------

  vsl_add_to_binary_loader(bgrl_vertex());
  vsl_add_to_binary_loader(bgrl_edge());
  
  // binary test output file stream
  vsl_b_ofstream bfs_out("test_graph_io.tmp");
  TEST("Created test_graph_io.tmp for writing",(!bfs_out), false);
  vsl_b_write(bfs_out, the_graph);
  bfs_out.close();

  bgrl_graph_sptr graph_in;

  // binary test input file stream
  vsl_b_ifstream bfs_in("test_graph_io.tmp");
  TEST("Opened test_graph_io.tmp for reading",(!bfs_in), false);
  vsl_b_read(bfs_in, graph_in);
  bfs_in.close();

  // remove the temporary file
  vpl_unlink ("test_graph_io.tmp");

  TEST("Testing size() on loaded graph",
       graph_in->size() == the_graph->size(),
       true);
}


void test_memory_leak(const bgrl_vertex_sptr& v1)
{
  // This smart pointer should be the only thing left referencing this vertex
  TEST("Testing for memory leaks", v1->get_references(), 1);
}


MAIN( test_graph )
{
  START( "bgrl_graph" );

  bgrl_vertex_sptr v1 = new bgrl_vertex();
  test_graph(v1);
  test_memory_leak(v1);
  SUMMARY();
}
