#ifndef mbl_minimum_spanning_tree_h_
#define mbl_minimum_spanning_tree_h_
//:
// \file
// \author Tim Cootes
// \brief Functions to compute minimum spanning trees from distance data

#include <vector>
#include <iostream>
#include <utility>
#include <vnl/vnl_matrix.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_point_3d.h>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: Compute the minimum spanning tree given a distance matrix
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const vnl_matrix<double>& D,
                              std::vector<std::pair<int,int> >& pairs);

//: Compute the minimum spanning tree of given points
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const std::vector<vgl_point_2d<double> >& pts,
                              std::vector<std::pair<int,int> >& pairs);

//: Compute the minimum spanning tree of given points
//  \param pairs[0].first is the root node
//  Tree defined by pairs.
//  \param pairs[i].second is linked to \param pairs[i].first
//  We compute the minimum spanning tree of the graph using Prim's algorithm.
void mbl_minimum_spanning_tree(const std::vector<vgl_point_3d<double> >& pts,
                              std::vector<std::pair<int,int> >& pairs);

#endif // mbl_minimum_spanning_tree_h_
