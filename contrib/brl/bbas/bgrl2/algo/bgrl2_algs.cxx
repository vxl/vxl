// This is brl/bbas/bgrl2/algo/bgrl2_algs.cxx
#include "bgrl2_algs.h"
//:
// \file
#include <bgrl2/bgrl2_vertex.h>
#include <bgrl2/bgrl2_edge.h>
#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//: return the euler tour of the graph starting from the given edge and in the direction towards the given node
//  (the given node should be either source or target of the given node)
//  ALG:
//  starting from the "next edge" of the given node wrt the given edge, add the next edges of successor nodes as a chain
//  stop when the initial edge is re-encountered
template <class G, class E, class V>
void euler_tour(vbl_smart_ptr<G> g, vbl_smart_ptr<E> e, vbl_smart_ptr<V> n, std::vector<vbl_smart_ptr<E> >& edges)
{
  vbl_smart_ptr<E> current_edge = e;
  vbl_smart_ptr<V> current_node = n;

  assert(current_edge->source() == current_node || current_edge->target() == current_node);

  do {
    edges.push_back(current_edge);
    current_edge = g->cyclic_adj_succ(current_edge, current_node);
    if (!current_edge) {
      edges.clear();
      return;
    }
    current_node = current_edge->opposite(current_node);
  } while (current_node && current_edge != e);

  return;
}


//: return the depth of the graph starting from the given node (!!!assumes there are no loops)
template <class G, class E, class V>
int depth_no_loop(vbl_smart_ptr<G> g, vbl_smart_ptr<E> e, vbl_smart_ptr<V> n)
{
  vbl_smart_ptr<V> current_node = n;
  vbl_smart_ptr<E> current_edge = e;
  vbl_smart_ptr<E> start_edge = e;

  if (current_node->degree() == 0)  // an isolated node
    return 0;

  if (current_node->degree() == 1 && start_edge)  // base condition
    return 0;

  int max_depth = 0;

  //: if start_edge is zero we're at the root of the graph, start with the first child
  if (!start_edge) {
    start_edge = g->first_in_edge(current_node);
    if (!start_edge)
      start_edge = g->first_out_edge(current_node);

    max_depth = depth_no_loop(g, start_edge, start_edge->opposite(current_node));
    if (current_node->degree() == 1)
      return max_depth + 1;
  }

  //: find the max depth of children except the child with the start edge
  current_edge = g->cyclic_adj_succ(start_edge, current_node);
  if (!current_edge)
    return -100000000;
  do {
    int depth = depth_no_loop(g, current_edge, current_edge->opposite(current_node));
    if (depth > max_depth)
      max_depth = depth;
    current_edge = g->cyclic_adj_succ(current_edge, current_node);
    if (!current_edge)
      return -100000000;
  } while (start_edge != current_edge);

  return max_depth + 1;
}

#undef DBGRL_EULER_TOUR_INSTANTIATE
#define DBGRL_EULER_TOUR_INSTANTIATE( G, E, V ) template void euler_tour(vbl_smart_ptr<G > g, vbl_smart_ptr<E > e, vbl_smart_ptr<V > n, std::vector<vbl_smart_ptr<E > >& edges)

#undef DBGRL_DEPTH_NO_LOOP_INSTANTIATE
#define DBGRL_DEPTH_NO_LOOP_INSTANTIATE( G, E, V ) template int depth_no_loop(vbl_smart_ptr<G > g, vbl_smart_ptr<E > e, vbl_smart_ptr<V > n)
