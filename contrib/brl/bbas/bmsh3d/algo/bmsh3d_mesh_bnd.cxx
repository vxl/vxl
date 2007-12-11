// This is brl/bbas/bmsh3d/algo/bmsh3d_mesh_bnd.cxx
//---------------------------------------------------------------------
#include "bmsh3d_mesh_bnd.h"
//:
// \file
// \brief Mesh boundary tracing.
//
// \author
//  MingChing Chang  Dec 14, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cassert.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

bool bmsh3d_bnd_chain::is_V_incident_via_HE (const bmsh3d_vertex* V)
{
  //Loop through all halfedges and check if V is on the bnd_chain.
  for (unsigned int i=0; i<HE_list_.size(); i++) {
    bmsh3d_halfedge* he = HE_list_[i];
    if (he->edge()->is_V_incident (V))
      return true;
  }
  return false;
}

//: Trace the bnd_chain into a polyline.
void bmsh3d_bnd_chain::trace_polyline (vcl_vector<vgl_point_3d<double> >& polyline_vertices)
{
  bmsh3d_vertex *V, *Vo;
  polyline_vertices.clear();

  assert (HE_list_.size() != 0);
  bmsh3d_halfedge* HE0 = HE_list (0);

  if (HE_list_.size() == 1) {
    //The case of only one boundary edge.
    polyline_vertices.push_back (HE0->edge()->sV()->pt());
    polyline_vertices.push_back (HE0->edge()->eV()->pt());
    return;
  }

  //First vertex
  bmsh3d_halfedge* HE1 = HE_list (1);
  V = incident_V_of_Es (HE0, HE1);
  Vo = HE0->edge()->other_V (V);
  polyline_vertices.push_back (Vo->pt());

  vcl_vector<bmsh3d_halfedge*>::iterator it = HE_list_.begin();
  for (unsigned int i=0; i<HE_list_.size()-1; i++) {
    HE0 = HE_list (i);
    HE1 = HE_list (i+1);
    V = incident_V_of_Es (HE0, HE1);
    polyline_vertices.push_back (V->pt());
  }

  //Last vertex
  Vo = HE1->edge()->other_V (V);
  polyline_vertices.push_back (Vo->pt());
}

//: trace the boundary chain starting from F.
void bmsh3d_bnd_chain::trace_bnd_chain (bmsh3d_halfedge* startHE)
{
  bmsh3d_halfedge* he = startHE;
  bmsh3d_vertex* V = startHE->edge()->sV ();

  do {
    HE_list_.push_back (he);
    he->face()->set_visited (true);

    //Find the next boundary halfedge;
    bmsh3d_halfedge* nextHE = V->m2_get_next_bnd_HE (he);
    V = nextHE->edge()->other_V (V);

    he = nextHE;
  }
  while (he != startHE);

  ///assert (HE_list_.size() >= 3);
}

// #############################################################

void bmsh3d_bnd_chain_set::detect_bnd_chains ()
{
  vul_printf (vcl_cerr, "  detect_bnd_chains():\n");

  //Reset all mesh face traversing flags.
  mesh_->reset_traverse_f ();

  //Loop through all mesh faces and trace mesh boundary edges.
  vcl_map<int, bmsh3d_face*>::iterator it = mesh_->facemap().begin();
  for (; it != mesh_->facemap().end(); it++) {
    bmsh3d_face* F = (*it).second;
    if (F->b_visited())
      continue;
    F->set_visited (true);

    TRIFACE_TYPE topo_type = F->tri_get_topo_type();

    switch (topo_type) {
    case TRIFACE_111: //Todo
    break;
    case TRIFACE_112: //Todo
    break;
    case TRIFACE_122: //Start a new boundary chain and trace it.
    {
      bmsh3d_bnd_chain* BC = _new_bnd_chain ();
      chainset_.push_back (BC);
      bmsh3d_halfedge* startHE = F->find_bnd_HE ();
      BC->trace_bnd_chain (startHE);
    }
    break;
    case TRIFACE_222: //Skip
    break;
    default:
    break;
    }
  }

  vul_printf (vcl_cerr, "\t%u boundary chain created.\n", chainset_.size());
}

//: remove bnd_chain larger than n edges.
void bmsh3d_bnd_chain_set::remove_large_bnd_chain (unsigned int th)
{
  vcl_vector<bmsh3d_bnd_chain*>::iterator it = chainset_.begin();
  while (it != chainset_.end()) {
    bmsh3d_bnd_chain* BC = (*it);
    if (BC->num_edges() <= th) {
      it++;
      continue; //Keep this bnd_chain.
    }
    else { //Remove this bnd_chain.
      if (it == chainset_.begin()) { //head
        chainset_.erase (it);
        it = chainset_.begin();
      }
      else {
        vcl_vector<bmsh3d_bnd_chain*>::iterator next = it;
        next--;
        chainset_.erase (it);
        it = ++next;
      }
    }
  }
}
