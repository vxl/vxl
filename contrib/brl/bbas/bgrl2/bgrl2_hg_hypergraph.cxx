// This is brl/bbas/bgrl2/bgrl2_hg_hypergraph.cxx
#include "bgrl2_hg_hypergraph.h"
//:
// \file
// author: Ming-Ching Chang
// date:   Apr 04, 2005

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

////////////////////////////////////////////////////////////////////
//: ===== Internal Low-level Graph operation (without handling connectivity) =====
bool bgrl2_hg_hypergraph::_internal_del_vertex(bgrl2_hg_vertex* vertex)
{
  assert (vertex->connecting_hyperedges().size() == 0);
  return bgrl2_hg_graph::_internal_del_vertex(vertex);
}

bool bgrl2_hg_hypergraph::_internal_del_edge(bgrl2_hg_edge* edge)
{
  assert (edge->connecting_hyperedges().size() == 0);
  return bgrl2_hg_graph::_internal_del_edge(edge);
}

void bgrl2_hg_hypergraph::_internal_add_hyperedge(bgrl2_hg_hyperedge* hyperedge)
{
  assert (hyperedge && hyperedge->id() > 0);
#if 0 // TODO
  hyperedges_.insert(std::pair<int, bgrl2_hg_hyperedge*>(hyperedge->id(), hyperedge));
#endif
}

bool bgrl2_hg_hypergraph::_internal_del_hyperedge(bgrl2_hg_hyperedge* hyperedge)
{
  assert (hyperedge->connecting_edges().size() == 0);
  assert (hyperedge->connecting_vertices().size() == 0);
#if 0
  int n_removed = hyperedges_.erase(hyperedge->id());
  return n_removed == 1;
#else
  return true;
#endif
}

////////////////////////////////////////////////////////////////////
//: ===== High-level Graph operation (handling connectivity) =====
bool bgrl2_hg_hypergraph::remove_vertex(bgrl2_hg_vertex* vertex)
{
  auto E_it = vertex->connecting_edges().begin();
  while (E_it != vertex->connecting_edges().end()) {
    bgrl2_hg_edge* edge = (*E_it);
    _internal_disconnect_edge_vertex(edge, vertex);
  }

  auto H_it = vertex->connecting_hyperedges().begin();
  while (H_it != vertex->connecting_hyperedges().end()) {
    bgrl2_hg_hyperedge* hyperedge = (*H_it);
    _internal_disconnect_hyperedge_vertex(hyperedge, vertex);
  }

  return _internal_del_vertex(vertex);
}

bool bgrl2_hg_hypergraph::remove_edge(bgrl2_hg_edge* edge)
{
  bgrl2_hg_vertex* sv = edge->connecting_s_vertex();
  bgrl2_hg_vertex* ev = edge->connecting_e_vertex();

  if (sv)
    _internal_disconnect_edge_vertex(edge, sv);
  if (ev)
    _internal_disconnect_edge_vertex(edge, ev);

  for (unsigned int i=0; i<edge->connecting_hyperedges().size(); i++) {
    bgrl2_hg_hyperedge* hyperedge = edge->connecting_hyperedges(i);
    _internal_disconnect_hyperedge_edge(hyperedge, edge);
  }

  return _internal_del_edge(edge);
}

bool bgrl2_hg_hypergraph::remove_hyperedge(bgrl2_hg_hyperedge* hyperedge)
{
  auto E_it = hyperedge->connecting_edges().begin();
  while (E_it != hyperedge->connecting_edges().end()) {
    bgrl2_hg_edge* edge = (*E_it);
    _internal_disconnect_hyperedge_edge(hyperedge, edge);
  }

  auto V_it = hyperedge->connecting_vertices().begin();
  while (V_it != hyperedge->connecting_vertices().end()) {
    bgrl2_hg_vertex* vertex = (*V_it);
    _internal_disconnect_hyperedge_vertex(hyperedge, vertex);
  }

  return _internal_del_hyperedge(hyperedge);
}

#if 0
bool bgrl2_hg_hypergraph::remove_vertex(int id)
{
  bgrl2_hg_vertex* vertex = vertices(id);
  if (vertex)
    return remove_vertex(vertex);
  else
    return false;
}

bool bgrl2_hg_hypergraph::remove_edge(int id)
{
  bgrl2_hg_edge* edge = edges(id);
  if (edge)
    return remove_edge(edge);
  else
    return false;
}

bool bgrl2_hg_hypergraph::remove_hyperedge(int id)
{
  bgrl2_hg_hyperedge* hyperedge = hyperedges(id);
  if (hyperedge)
    return remove_hyperedge(hyperedge);
  else
    return false;
}

bool bgrl2_hg_hypergraph::topo_remove_vertex(int id)
{
  bgrl2_hg_vertex* vertex = vertices(id);
  if (vertex)
    return topo_remove_vertex(vertex);
  else
    return false;
}

bool bgrl2_hg_hypergraph::topo_remove_edge(int id)
{
  bgrl2_hg_edge* edge = edges(id);
  if (edge)
    return topo_remove_edge(edge);
  else
    return false;
}

bool bgrl2_hg_hypergraph::topo_remove_hyperedge(int id)
{
  //: for bgrl2_hg_hypergraph, can remove the hyperedge directly.
  return remove_hyperedge(id);
}
#endif // 0


////////////////////////////////////////////////////////////////////
//: ===== High-level Graph operation that the user should use =====
bool bgrl2_hg_hypergraph::topo_remove_vertex(bgrl2_hg_vertex* vertex)
{
  // ! Should check both edges and hyperedges!!
  if (vertex->connecting_edges().size() == 0) {
    assert (vertex->connecting_hyperedges().size() == 0);
    return remove_vertex(vertex);
  }
  else
    return false;
}

bool bgrl2_hg_hypergraph::topo_remove_edge(bgrl2_hg_edge* edge)
{
  if (edge->connecting_hyperedges().size() == 0)
    return remove_edge(edge);
  else
    return false;
}

bool bgrl2_hg_hypergraph::topo_remove_hyperedge(bgrl2_hg_hyperedge* hyperedge)
{
  //: for bgrl2_hg_hypergraph, can remove the hyperedge directly.
  return remove_hyperedge(hyperedge);
}
