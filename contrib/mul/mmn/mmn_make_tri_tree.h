#ifndef mmn_make_tri_tree_h_
#define mmn_make_tri_tree_h_
//:
// \file
// \brief Compute arcs defining a graph s.t. triangles form a tree.
// \author Tim Cootes

#include <iostream>
#include <vector>
#include <vnl/vnl_matrix.h>
#include <mmn/mmn_arc.h>
#include <mmn/mmn_triplet.h>
#include <mmn/mmn_dependancy.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute arcs defining a graph s.t. triangles form a tree.
//  Compute arc of graph such that point belongs to at least one triangle,
//  and the graph of triangles is a tree (acyclic).
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
//  \param v0: If input as < D.rows() then defines one node of the first arc
void mmn_make_tri_tree(const vnl_matrix<double>& D,
                       std::vector<mmn_arc>& arcs,
                       unsigned int v0 = (unsigned int)(-1));

//: Compute arcs defining a graph s.t. triangles form a tree.
//  Compute arc of graph such that point belongs to at least one triangle,
//  and the graph of triangles is a tree (acyclic).
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
//  \param triplets: n-2 triplets defining triangles
//  \param deps: n-1 dependancies, defining a way to traverse graph
//  \param v0: If input as < D.rows() then defines one node of the first arc
void mmn_make_tri_tree(const vnl_matrix<double>& D,
                       std::vector<mmn_arc>& arcs,
                       std::vector<mmn_triplet>& triplets,
                       std::vector<mmn_dependancy>& deps,
                       unsigned int v0 = (unsigned int)(-1));


#endif // mmn_make_tri_tree_h_
