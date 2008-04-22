#ifndef mmn_graph_rep1_h_
#define mmn_graph_rep1_h_
//:
// \file
// \brief Representation of a graph, stored by links at each node.
// \author Tim Cootes

#include <mmn/mmn_arc.h>
#include <mmn/mmn_dependancy.h>
#include <vcl_vector.h>
#include <vcl_utility.h>  // For vcl_pair

//: Representation of a graph, stored by links at each node.
//  Optimised for adding arcs and finding arcs for each node.
//  Assumes that there is an ordered list of arcs (but doesn't
//  actually record it explicitly).
class mmn_graph_rep1
{
 private:
  //: Indicates arcs connected to each node
  //  node_data_[i][j].first == vertex connected to node i
  //. *.second == index of arc which does the connection.
  vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > > node_data_;

#if 0
  //: Number of options for each node
  //  Used to select most efficient simplification of the graph
  vcl_vector<unsigned> n_;
#endif // 0

  //: Maximum number of arcs used
  unsigned max_n_arcs_;

  //: Current number of arcs
  unsigned n_arcs_;

  //: Index of node chosen to be root (if >n_nodes,then none chosen)
  unsigned root_index_;

  //: Remove record of arc v1-v2 from v1
  void remove_arc_from_node(unsigned v1, unsigned v2);
 public:
  //: Default constructor
  mmn_graph_rep1();

  //: Indicates arcs connected to each node
  //  node_data_[i][j].first == vertex connected to node i
  //. *.second == index of arc which does the connection.
  const vcl_vector<vcl_vector<vcl_pair<unsigned,unsigned> > >& node_data() const
  { return node_data_; }

  //: Maximum number of distinct arcs used
  unsigned max_n_arcs() const { return max_n_arcs_; }

  //: Current number of arcs
  unsigned n_arcs() const { return n_arcs_; }

  //: Build from list of arcs
  void build(unsigned n_nodes, const vcl_vector<mmn_arc>& arcs);

  //: Return index of arc between v1 and v2, or -1 if none
  int arc_index(unsigned v1, unsigned v2) const;

  //: Return index of arc between v1 and v2, creating one if none exists
  unsigned get_arc(unsigned v1, unsigned v2);

  //: Remove some of leaves of graph, recording dependencies
  //  A leaf node is one with only one arc
  //  For each leaf node removed, add one dependency object to
  //  the deps list.
  //  Returns number of leaves removed.
  unsigned remove_leaves(vcl_vector<mmn_dependancy>& deps);

  //: Remove all of leaves of graph, recording dependencies
  //  A leaf node is one with only one arc
  //  For each leaf node removed, add one dependency object to
  //  the deps list.  On exit, this graph has no leaves.
  //  Returns number of leaves removed.
  unsigned remove_all_leaves(vcl_vector<mmn_dependancy>& deps);

  //: Remove arcs from some of the nodes with two neighbours
  //  Record the pairwise dependencies.
  //  For each node removed, add one dependency object to
  //  the deps list.
  //  Returns number of removed.
  unsigned remove_pair_deps(vcl_vector<mmn_dependancy>& deps);

  //: Compute list of all single and pairwise dependencies
  //  Finds ordered list of dependencies.
  //  If returns true, then dep is an ordered list of dependencies
  //  allowing us solve a minimisation problem one node at a time.
  //  If it returns false, then the graph cannot be decomposed into
  //  a sequence of single or pairwise dependencies.
  //  If dep[i].n_dep==1 for all i, then the graph is a tree, and
  //  reversing the order of dep gives a means of traversing from the
  //  root to the leaves.  The original order gives a method of
  //  visiting every node only after any child/leaf nodes have been
  //  visited first.
  //  Destroys current structure in the process.
  //
  //  root_index indicates which node is to be the root for the
  //  resulting tree (ie the one node remaining - defined in the
  //  v1 of the last element in deps).
  bool compute_dependancies(vcl_vector<mmn_dependancy>& deps,
                            unsigned root_index);

  //: Compute list of all single and pairwise dependencies
  //  As compute_dependancies(deps,root), but root selected by algorithm
  bool compute_dependancies(vcl_vector<mmn_dependancy>& deps);

};

#endif // mmn_graph_rep1_h_
