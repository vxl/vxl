// This is brl/bbas/bgrl2/tests/test_graph.h
#ifndef test_graph_h_
#define test_graph_h_
//:
// \file
// \brief Test code for using the templated graph class
// \author Amir Tamrakar
// \date June 18, 2005

#include <bgrl2/bgrl2_graph.h>

#include "test_edge.h"
#include "test_node.h"

//class test_node;
//class test_edge;

//: a test graph class subclassed from bgrl2_graph
class test_graph : public bgrl2_graph<test_node, test_edge>
{
 public:
  test_graph() : bgrl2_graph<test_node, test_edge>() {}
  ~test_graph() override = default;
};

#endif //test_graph_h_
