#ifndef mbl_minimum_spanning_tree_h_
#define mbl_minimum_spanning_tree_h_

//:
// \file
// \author Tim Cootes
// \brief Functions to compute minimum spanning trees from distance data

#include <vnl/vnl_matrix.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#include <vcl_vector.h>
#include <vcl_utility.h>  // For vcl_pair

//: Compute the minimum spanning tree given a distance matrix
//  pairs[0].first is the root node
//  Tree defined by pairs.  pairs[i].second is linked to pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vnl_matrix<double>& D,
                              vcl_vector<vcl_pair<int,int> >& pairs);

//: Compute the minimum spanning tree of given points
//  pairs[0].first is the root node
//  Tree defined by pairs.  pairs[i].second is linked to pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vcl_vector<vgl_point_2d<double> >& pts,
                              vcl_vector<vcl_pair<int,int> >& pairs);

//: Compute the minimum spanning tree of given points
//  pairs[0].first is the root node
//  Tree defined by pairs.  pairs[i].second is linked to pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vcl_vector<vgl_point_3d<double> >& pts,
                              vcl_vector<vcl_pair<int,int> >& pairs);

#endif // mbl_minimum_spanning_tree_h_


