//:
// \file
// \brief Analyze a graph to deduce the dependency order.
// \author Tim Cootes

#include <mmn/mmn_analyze_graph.h>
#include <mmn/mmn_graph_rep1.h>

//: Given a graph with n.size() nodes and arc.size() arcs, deduce dependencies
//  If returns true, then dep is an ordered list of dependencies
//  allowing us solve a minimisation problem one node at a time.
//  If it returns false, then the graph cannot be decomposed into
//  a sequence of single or pairwise dependencies.
//  If dep[i].n_dep==1 for all i, then the graph is a tree, and
//  reversing the order of dep gives a means of traversing from the
//  root to the leaves.  The original order gives a method of 
//  visiting every node only after any child/leaf nodes have been
//  visited first.
bool mmn_analyze_graph(const vcl_vector<unsigned>& n,
                       const vcl_vector<mmn_arc>& arc,
                       vcl_vector<mmn_dependancy>& dep,
                       unsigned& max_n_arcs)
{
  mmn_graph_rep1 g;
  g.build(n.size(),arc);
  bool b = g.compute_dependancies(dep);
  max_arcs = g.max_n_arcs();
  return b;
}
