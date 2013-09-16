// This is core/vbl/vbl_graph_partition.h
#ifndef vbl_graph_partition_h_
#define vbl_graph_partition_h_
#ifdef VCL_NEEDS_PRAGMA_INTERFACE
#pragma interface
#endif
//:
// \file
// \brief Partitions a graph into disjoint connected components
// \author J. Mundy
// \date February 14, 2013
// \verbatim
// \endverbatim
//-----------------------------------------------------------------------------
// Adapted from the paper and code by Pedro Felzenszwalb
// International Journal of Computer Vision, Vol. 59, No. 2, September 2004
// The graph is represented by a set of integer vertex ids and edges that
// are a pair of vertices. Each edge has an associated weight. Edges
// are sorted according to weight and then connected components are
// merged if the difference in internal component weights are greater
// than the smallest weight edge between a pair of adjacent components.
// The result is a set of isolated sub-graphs of the original graph.
#include <vcl_vector.h>
#include <vbl/vbl_disjoint_sets.h>
#include <vbl/vbl_edge.h>
#include <vil/vil_image_view.h>

//:
// \p t is a constant that determines the threshold on edge weight
//  to form disconnected sets
void vbl_graph_partition(vbl_disjoint_sets& ds, vcl_vector<vbl_edge>& edges, float t);

// a partitioning function that uses an edge image so that the component shapes follow edges as much as possible
void vbl_graph_partition(vbl_disjoint_sets& ds, vcl_vector<vbl_edge>& edges, float t, vil_image_view<float>& edge_img);

#endif // vbl_graph_partition_h_
