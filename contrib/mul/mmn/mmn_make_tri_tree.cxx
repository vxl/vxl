//:
// \file
// \brief Representation of topological arc joining two vertices
// \author Tim Cootes

#include <mmn/mmn_make_tri_tree.h>
#include <vcl_cassert.h>

static void  update_best_arcs(const vnl_matrix<double>& D,
                              const vcl_vector<bool>& node_free,
                              vcl_vector<mmn_arc>& arcs, unsigned a,
                              vcl_vector<unsigned>& best_arc,
                              vcl_vector<double>& best_d)
{
  unsigned v1=arcs[a].v1;
  unsigned v2=arcs[a].v2;
  for (unsigned i=0;i<node_free.size();++i)
  {
    if (node_free[i])
    {
      double d = D(i,v1)+D(i,v2);
      if (d<best_d[i])
      {
        best_d[i]=d;
        best_arc[i]=a;
      }
    }
  }
}

//: Compute arcs defining a graph s.t. triangles form a tree.
//  Compute arc of graph such that point belongs to at least one triangle,
//  and the graph of triangles is a tree (acylcic).
//  Two triangles are neighbours if they share an edge (arc).
//
//  The approach is to select nodes one at a time, at each step
//  choosing the node closest to two nodes ending an existing arc.
//  This gives two new arcs.
//
//  Complexity is approximately O(n^2)
//
//  \param D: a symmetric matrix indicating proximity of two nodes
//  \param arcs: Output 2n-3 arcs defining the graph.
//  \param v0: If input as >=0 then defines one node of the first arc
void mmn_make_tri_tree(const vnl_matrix<double>& D,
                       vcl_vector<mmn_arc>& arcs,
                       int v0)
{
  unsigned n = D.rows();
  assert(D.cols()==n);

  vcl_vector<bool> node_free(n,true);

  // Record of index of best arc for each node
  // arcs[best_arc[i]] is arc whose nodes are closest to i
  // best_d[i] is the sum of D(i,v1)+D(i,v2)
  vcl_vector<unsigned> best_arc(n);
  vcl_vector<double> best_d(n);

  arcs.resize(0);

  // Deduce the first arc
  mmn_arc arc(0,1);

  if (v0>=0 && v0<n)
  {
    // Find nearest neighbour of v0
    double min_d=9e9;
    unsigned best_i;
    for (unsigned i=0;i<n;++i)
      if (i!=v0 && D(v0,i)<min_d)
      { best_i=i; min_d=D(v0,i); }
    arc = mmn_arc(v0,best_i);
  }
  else
  {
    // Find smallest element in D
    double min_d = D(arc.v1,arc.v2);
    for (unsigned j=1;j<n;++j)
      for (unsigned i=0;i<j;++j)
      {
        if (D(i,j)<min_d)
        {
          arc=mmn_arc(i,j); min_d=D(i,j);
        }
      }
  }
  arcs.push_back(arc);
  node_free[arc.v1]=false;
  node_free[arc.v2]=false;

  // Initialise list of best arcs and distances
  for (unsigned i=0;i<n;++i)
  {
    best_arc[i]=0;
    if (node_free[i]) best_d[i] = D(i,arc.v1)+D(i,arc.v2);
  }

  for (unsigned k=2;k<n;++k)
  {
    // Search for node with lowest distance to an arc end
    unsigned best_i=0;
    double min_d=9e9;
    for (unsigned i=0;i<n;++i)
      if (node_free[i] && best_d[i]<min_d)
        {min_d=best_d[i]; best_i=i; }

    arc = arcs[best_arc[best_i]];
    // Record arcs from node best_i to ends of arc best_arcs[best_i]
    node_free[best_i]=false;
    arcs.push_back(mmn_arc(best_i,arc.v1));
    arcs.push_back(mmn_arc(best_i,arc.v2));

    // Re-evaluate distances to each free point
    update_best_arcs(D,node_free,arcs,arcs.size()-2,best_arc,best_d);
    update_best_arcs(D,node_free,arcs,arcs.size()-1,best_arc,best_d);
  }
}
