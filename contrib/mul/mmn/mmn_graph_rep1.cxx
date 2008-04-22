#include "mmn_graph_rep1.h"
//:
// \file
// \brief Representation of a graph, stored by links at each node.
// \author Tim Cootes

#include <vcl_cassert.h>

//: Default constructor
mmn_graph_rep1::mmn_graph_rep1()
  : max_n_arcs_(0), n_arcs_(0)
{
}

//: Build from list of arcs
void mmn_graph_rep1::build(unsigned n_nodes,
                           const vcl_vector<mmn_arc>& arcs)
{
  node_data_.resize(n_nodes);
  for (unsigned i=0;i<n_nodes;++i) node_data_[i].resize(0);

  for (unsigned i=0;i<arcs.size();++i)
  {
    node_data_[arcs[i].v1].push_back(vcl_pair<unsigned,unsigned>(arcs[i].v2,i));
    node_data_[arcs[i].v2].push_back(vcl_pair<unsigned,unsigned>(arcs[i].v1,i));
  }

  max_n_arcs_ = arcs.size();
  n_arcs_ = arcs.size();

  // root node not chosen, so allow automatic selection
  root_index_ = n_nodes+1;
}

//: Return index of arc between v1 and v2, or -1 if none
int mmn_graph_rep1::arc_index(unsigned v1, unsigned v2) const
{
  const vcl_vector<vcl_pair<unsigned,unsigned> >& nd = node_data_[v1];
  for (unsigned i=0;i<nd.size();++i)
    if (nd[i].first==v2) return nd[i].second;
  return -1;
}

//: Return index of arc between v1 and v2, creating one if none exists
unsigned mmn_graph_rep1::get_arc(unsigned v1, unsigned v2)
{
  int a = arc_index(v1,v2);
  if (a>=0) return a;

  // No existing arc, so add one.
  a = max_n_arcs_;
  node_data_[v1].push_back(vcl_pair<unsigned,unsigned>(v2,a));
  node_data_[v2].push_back(vcl_pair<unsigned,unsigned>(v1,a));
  max_n_arcs_++;
  n_arcs_++;
  return a;
}

#if 0
//: Remove record of arc between v1 and v2
//  Returns false if there isn't one.
bool mmn_graph_rep1::remove_arc(unsigned v1, unsigned v2)
{
}
#endif // 0

//: Remove record of arc v1-v2 from v1
void mmn_graph_rep1::remove_arc_from_node(unsigned v1, unsigned v2)
{
  vcl_vector<vcl_pair<unsigned,unsigned> >& nd = node_data_[v1];
  vcl_vector<vcl_pair<unsigned,unsigned> >::iterator ndi;
  for (ndi=nd.begin();ndi!=nd.end();++ndi)
    if (ndi->first==v2)
    {
      nd.erase(ndi);
      break;
    }
}


//: Remove some of leaves of graph, recording dependencies
//  A leaf node is one with only one arc
//  For each leaf node removed, add one dependency object to
//  the deps list.
//  Returns number of leaves removed.
unsigned mmn_graph_rep1::remove_leaves(vcl_vector<mmn_dependancy>& deps)
{
  unsigned n_removed = 0;
  for (unsigned v1=0;v1<node_data_.size();++v1)
  {
    if (v1==root_index_) continue;  // Don't remove the root

    if (node_data_[v1].size()==1)
    {
      // v1 is a leaf node, connected to node_data_[v1][0].first
      unsigned v2 = node_data_[v1][0].first;
      unsigned arc12 = node_data_[v1][0].second;

      // Record dependency
      deps.push_back(mmn_dependancy(v1,v2,arc12));
      n_removed++;

      // Remove the record of the arc from v1
      node_data_[v1].resize(0);
      // Remove the record of the arc from v2
      remove_arc_from_node(v2,v1);

      n_arcs_--;
      if (n_arcs_==0) break;
    }
  }

  return n_removed;
}

//: Remove all of leaves of graph, recording dependencies
//  A leaf node is one with only one arc
//  For each leaf node removed, add one dependency object to
//  the deps list.  On exit, this graph has no leaves.
//  Returns number of leaves removed.
unsigned mmn_graph_rep1::remove_all_leaves(vcl_vector<mmn_dependancy>& deps)
{
  unsigned n_removed=0;
  unsigned nr=0;
  do
  {
    nr=remove_leaves(deps);
    n_removed+=nr;
  } while (nr!=0);

  return n_removed;
}

//: Remove arcs from some of the nodes with two neighbours
//  Record the pairwise dependencies.
//  For each node removed, add one dependency object to
//  the deps list.
//  Returns number of removed.
unsigned mmn_graph_rep1::remove_pair_deps(vcl_vector<mmn_dependancy>& deps)
{
  unsigned n_removed = 0;
  for (unsigned v0=0;v0<node_data_.size();++v0)
  {
    if (v0==root_index_) continue;  // Don't remove the root

    if (node_data_[v0].size()==2)
    {
      // v0 has two neighbours,
      // node_data_[v0][0].first and node_data_[v0][1].first
      unsigned v1 = node_data_[v0][0].first;
      unsigned arc1 = node_data_[v0][0].second;
      unsigned v2 = node_data_[v0][1].first;
      unsigned arc2 = node_data_[v0][1].second;

      // Find arc between v1-v2, or create one if necessary
      unsigned arc12 = get_arc(v1,v2);

      // Record dependency
      // If one of v1,v2 is root_index, then re-arrange so that it is v1
      if (v2==root_index_)
        deps.push_back(mmn_dependancy(v0,v2,v1,arc2,arc1,arc12));
      else
        deps.push_back(mmn_dependancy(v0,v1,v2,arc1,arc2,arc12));
      n_removed++;

      // Remove the record of the arcs from v0
      node_data_[v0].resize(0);
      // Remove the record of the arc from v1
      remove_arc_from_node(v1,v0);
      // Remove the record of the arc from v2
      remove_arc_from_node(v2,v0);

      n_arcs_-=2;
      if (n_arcs_<2) break;
    }
  }

  return n_removed;
}

//: Compute list of all single and pairwise dependencies
//  Return true if graph can be fully decomposed in this way
bool mmn_graph_rep1::compute_dependancies(vcl_vector<mmn_dependancy>& deps, unsigned root_index)
{
  assert(root_index<node_data_.size());

  root_index_=root_index;

  deps.resize(0);
  unsigned nr1=0;
  do
  {
    nr1=remove_all_leaves(deps);
    if (n_arcs_>1)
      nr1+=remove_pair_deps(deps);
  }
  while (nr1>0 && n_arcs_>0);

  return n_arcs_==0;
}

//: Compute list of all single and pairwise dependencies
//  Return true if graph can be fully decomposed in this way
bool mmn_graph_rep1::compute_dependancies(vcl_vector<mmn_dependancy>& deps)
{
  // Indicate that root index is undefined.
  root_index_=node_data_.size()+1;

  deps.resize(0);
  unsigned nr1=0;
  do
  {
    nr1=remove_all_leaves(deps);
    if (n_arcs_>1)
      nr1+=remove_pair_deps(deps);
  }
  while (nr1>0 && n_arcs_>0);

  return n_arcs_==0;
}

