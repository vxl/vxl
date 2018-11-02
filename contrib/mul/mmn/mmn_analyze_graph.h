#ifndef mmn_analyze_graph_h_
#define mmn_analyze_graph_h_

//:
// \file
// \brief Analyze a graph to deduce the dependency order.
// \author Tim Cootes

#include <iostream>
#include <vector>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

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
bool mmn_analyze_graph(const std::vector<unsigned>& n,
                       const std::vector<mmn_arc>& arc,
                       std::vector<mmn_dependancy>& dep,
                       unsigned& max_arcs);


#endif // mmn_analyze_graph_h_
