//:
// \file
#include "vbl_graph_partition.h"
#include <algorithm>
#include <vcl_compiler.h>
#include <cmath>

//:
// \p t is a constant that determines the threshold on edge weight
//  to form disconnected sets
void vbl_graph_partition(vbl_disjoint_sets& ds, std::vector<vbl_edge>& edges, float t) {
   // sort edges by weight in increasing order
   std::sort(edges.begin(), edges.end());


  int nv = ds.num_elements();
  // init thresholds to t
  std::vector<float> thr(nv, t);

  int ne = (int)edges.size();
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


// a partitioning function that uses an edge image so that the component shapes follow edges as much as possible
void vbl_graph_partition(vbl_disjoint_sets& ds, std::vector<vbl_edge>& edges, float t, vil_image_view<float>& edge_img)
{
  // sort edges by weight in increasing order
   std::sort(edges.begin(), edges.end());


  int nv = ds.num_elements();
  // init thresholds to t
  std::vector<float> thr(nv, t);

  int ne = (int)edges.size();
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

