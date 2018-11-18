// This is brl/bbas/bgrl2/bgrl2_hg_graph.cxx
#include <iostream>
#include <utility>
#include "bgrl2_hg_graph.h"
//:
// \file
// author: Ming-Ching Chang
// date:   Apr 04, 2005

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

////////////////////////////////////////////////////////////////////
// ===== Internal Low-level Graph operation (without handling connectivity) =====
void bgrl2_hg_graph::_internal_add_vertex (bgrl2_hg_vertex* vertex)
{
  assert (vertex->id() > 0);
  vertices_.insert (std::pair<int, bgrl2_hg_vertex*> (vertex->id(), vertex));
}

bool bgrl2_hg_graph::_internal_del_vertex (bgrl2_hg_vertex* vertex)
{
  assert (vertex->connecting_edges().size()==0);
  int n_removed = vertices_.erase (vertex->id());
  return n_removed == 1;
}

void bgrl2_hg_graph::_internal_add_edge (bgrl2_hg_edge* edge)
{
  assert (edge->id() > 0);
  edges_.insert (std::pair<int, bgrl2_hg_edge*> (edge->id(), edge));
}

bool bgrl2_hg_graph::_internal_del_edge (bgrl2_hg_edge* edge)
{
  assert (edge->connecting_s_vertex() == nullptr &&
          edge->connecting_e_vertex() == nullptr);
  int n_removed = edges_.erase (edge->id());
  return n_removed == 1;
}

////////////////////////////////////////////////////////////////////
// ===== High-level Graph operation (handling connectivity) =====

bool bgrl2_hg_graph::remove_vertex (bgrl2_hg_vertex* vertex)
{
  auto it = vertex->connecting_edges().begin();
  while (it != vertex->connecting_edges().end()) {
    bgrl2_hg_edge* edge = (*it);
    _internal_disconnect_edge_vertex (edge, vertex);
  }

  return _internal_del_vertex (vertex);
}

bool bgrl2_hg_graph::remove_vertex (int id)
{
  bgrl2_hg_vertex* vertex = vertices (id);
  if (vertex)
    return remove_vertex (vertex);
  else
    return false;
}

bool bgrl2_hg_graph::remove_edge (bgrl2_hg_edge* edge)
{
  bgrl2_hg_vertex* sv = edge->connecting_s_vertex();
  bgrl2_hg_vertex* ev = edge->connecting_e_vertex();

  _internal_disconnect_edge_vertex (edge, sv);
  _internal_disconnect_edge_vertex (edge, ev);

  return _internal_del_edge (edge);
}

bool bgrl2_hg_graph::remove_edge (int id)
{
  bgrl2_hg_edge* edge = edges (id);
  if (edge)
    return remove_edge (edge);
  else
    return false;
}

////////////////////////////////////////////////////////////////////
// ===== High-level Graph operation that the user should use =====

//: Only remove the svertex if it is an isolated vertex (no scaffold_curves connecting to it!).
bool bgrl2_hg_graph::topo_remove_vertex (bgrl2_hg_vertex* vertex)
{
  if (vertex->connecting_edges().size() == 0)
    return remove_vertex (vertex);
  else
    return false;
}

bool bgrl2_hg_graph::topo_remove_vertex (int id)
{
  auto* vertex = (bgrl2_hg_vertex*) vertices (id);
  if (vertex)
    return topo_remove_vertex (vertex);
  else
    return false;
}

bool bgrl2_hg_graph::topo_remove_edge (bgrl2_hg_edge* edge)
{
  //: for scaffold_graph, can remove the graph edge directly.
  return remove_edge (edge);
}

bool bgrl2_hg_graph::topo_remove_edge (int id)
{
  //: for scaffold_graph, can remove the graph edge directly.
  return remove_edge (id);
}
