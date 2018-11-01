// This is core/vbl/vbl_graph_partition.h
#ifndef vbl_graph_partition_h_
#define vbl_graph_partition_h_
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
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vbl/vbl_disjoint_sets.h>
#include <vbl/vbl_edge.h>
#include <vil/vil_image_view.h>

//:
// \p t is a constant that determines the threshold on edge weight
//  to form disconnected sets
void vbl_graph_partition(vbl_disjoint_sets& ds, std::vector<vbl_edge>& edges, float t);


#endif // vbl_graph_partition_h_
