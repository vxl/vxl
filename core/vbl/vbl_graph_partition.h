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
#include <vcl_algorithm.h>
#include <vcl_cmath.h>
#include <vbl/vbl_disjoint_sets.h>
#include <vbl/vbl_edge.h>

//:
// \p t is a constant that determines the threshold on edge weight
//  to form disconnected sets
void vbl_graph_partition(vbl_disjoint_sets& ds, vcl_vector<vbl_edge>& edges,
                         float t) {
   // sort edges by weight in increasing order
   vcl_sort(edges.begin(), edges.end());


  int nv = ds.num_elements();
  // init thresholds to t
  vcl_vector<float> thr(nv, t);

  int ne = edges.size();
  for (int i = 0; i < ne; i++) {
    vbl_edge& e = edges[i];

    // the roots of the partitions conected by this edge
    int v0 = ds.find_set(e.v0_);
    int v1=  ds.find_set(e.v1_);
    // if not the same partition
    if (v0 != v1) {
      // if the edge weight is lower than the thresholds of each partition
      if ((e.w_ <= thr[v0]) &&
          (e.w_ <= thr[v1])) {
        ds.set_union(v0, v1);// merge the two partitions
        v0= ds.find_set(v0);// find the root of the merged set
        thr[v0] = e.w_ + (t/ds.size(v0));//adapt the threshold
        //eventually the threshold is just the highest edge weight in the set
      }
    }
  }
}

#endif // vbl_graph_partition_h_
