#ifndef mmn_analyze_graph_h_
#define mmn_analyze_graph_h_

//:
// \file
// \brief Analyze a graph to deduce the dependancy order.
// \author Tim Cootes

#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <vcl_vector.h>

//: Given a graph with n.size() nodes and arc.size() arcs, deduce dependancies
//  If returns true, then dep is an ordered list of dependancies
//  allowing us solve a minimisation problem one node at a time.
//  If it returns false, then the graph cannot be decomposed into
//  a sequence of single or pairwise dependancies.
//  If dep[i].n_dep==1 for all i, then the graph is a tree, and
//  reversing the order of dep gives a means of traversing from the
//  root to the leaves.  The original order gives a method of 
//  visiting every node only after any child/leaf nodes have been
//  visited first.
bool mmn_analyze_graph(const vcl_vector<unsigned>& n,
                       const vcl_vector<mmn_arc>& arc,
                       vcl_vector<mmn_dependancy>& dep,
                       unsigned& max_arcs);



#endif // mmn_analyze_graph_h_
