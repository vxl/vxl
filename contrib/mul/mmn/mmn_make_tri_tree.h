#ifndef mmn_make_tri_tree_h_
#define mmn_make_tri_tree_h_
//:
// \file
// \brief Representation of topological arc joining two vertices
// \author Tim Cootes


#include <vnl/vnl_matrix.h>
#include <mmn/mmn_arc.h>
#include <vcl_vector.h>

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
                       int v0=-1);


#endif // mmn_make_tri_tree_h_

