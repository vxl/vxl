// This is brl/bbas/bmsh3d/bmsh3d_graph.cxx
//---------------------------------------------------------------------
#include <iostream>
#include "bmsh3d_graph.h"
//:
// \file
// \brief Graph
//
// \author
//  MingChing Chang  July 05, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vul/vul_printf.h>

//###############################################################
//    GRAPH
//###############################################################

////////////////////////////////////////////////////////////////////
//: Validation & Report

bool bmsh3d_graph::brute_force_validation ()
{
  bool result = true;

#if 0
  //: validate if there's any self A13 loop
  std::map<int, bmsh3d_edge*>::iterator SC_it = edgemap_.begin();
  for (; SC_it != edgemap_.end(); SC_it++) {
    bmsh3d_edge* SCurve = (*SC_it).second;

    if (SCurve->type() == bmsh3d_edge::SC_TYPE_A13_AXIAL &&
        SCurve->is_self_loop()) {
      vul_printf (std::cerr, "\tA13 scaffold curve (%d: %d-%d) has self-loop\n",
                  SCurve->id(),
                  SCurve->S_Vertex()->id(),
                  SCurve->E_Vertex()->id());
      result = false;
    }
  }
#endif
  return result;
}

void bmsh3d_graph::summary_report ()
{
}

//: Remove all loops of the input graph G.
void remove_graph_loops (bmsh3d_graph* G)
{
  vul_printf (std::cerr, "remove_graph_loops():\n");
  unsigned int count = 0;
  std::map<int, bmsh3d_edge*>::iterator it = G->edgemap().begin();
  while (it != G->edgemap().end()) {
    bmsh3d_edge* E = (*it).second;
    if (E->is_self_loop()) {
      vul_printf (std::cerr, "\tremoving graph edge %d.\n", E->id());
      std::map<int, bmsh3d_edge*>::iterator tmp = it;
      tmp++;
      count++;
      G->remove_edge (E);
      it = tmp;
    }
    else
      it++;
  }
  vul_printf (std::cerr, "\tdone. %u loop(s) removed.\n", count);
}

void clone_graph (bmsh3d_graph* targetG, bmsh3d_graph* inputG)
{
  clone_ptset (targetG, inputG);

  //Clone all graph edges.
  std::map<int, bmsh3d_edge*>::iterator it = inputG->edgemap().begin();
  for (; it != inputG->edgemap().end(); it++) {
    bmsh3d_edge* E = (*it).second;
    bmsh3d_vertex* sV = E->vertices(0);
    bmsh3d_vertex* eV = E->vertices(1);
    bmsh3d_vertex* newSV = targetG->vertexmap (sV->id());
    bmsh3d_vertex* newEV = targetG->vertexmap (eV->id());
    bmsh3d_edge* newE = targetG->_new_edge (newSV, newEV, E->id());
    targetG->add_edge_incidence_check (newE); //faster version: add_edge_incidence()
  }
  targetG->set_edge_id_counter (inputG->edge_id_counter());
}
