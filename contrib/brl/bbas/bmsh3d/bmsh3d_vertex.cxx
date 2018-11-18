// This is brl/bbas/bmsh3d/bmsh3d_vertex.cxx
//---------------------------------------------------------------------
#include <sstream>
#include <iostream>
#include "bmsh3d_vertex.h"
//:
// \file
// \brief Basic 3d point sample
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   Peter Vanroose - 6 Sep 2008 - avoid const-casting-away warnings
// \endverbatim
//
//-------------------------------------------------------------------------

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bmsh3d_edge.h"
#include "bmsh3d_face.h"

//: function to return all incident faces of this vertex
int bmsh3d_vertex::get_incident_Fs(std::set<bmsh3d_face*>& face_set)
{
  //: loop through all incident edges and put all faces into the set.
  for (bmsh3d_ptr_node* cur = E_list_; cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    //loop through all incident halfedges of this edge and get the associated faces
    if (E->halfedge() && E->halfedge()->pair() == nullptr)
    {
      face_set.insert(E->halfedge()->face());
    }
    else if (E->halfedge()) // the associated halfedges form a circular list
    {
      bmsh3d_halfedge* HE = E->halfedge();
      do {
        face_set.insert(HE->face());
        HE = HE->pair();
      }
      while (HE != E->halfedge());
    }
  }
  return face_set.size();
}

//: Detect the typology of a mesh vertex.
//  VTOPO_ISOLATED: no incident edge.
//  VTOPO_EDGE_ONLY: any edge has no incident halfedge.
//  VTOPO_EDGE_JUNCTION: any incident edge has more than 2 faces.
//  VTOPO_2_MANIFOLD: one check_2_manifold_() loop to finish all traversing, start_e != end_e.
//  VTOPO_2_MANIFOLD_1RING: one check_2_manifold_() loop to finish all traversing, start_e == end_e.
//  VTOPO_NON_MANIFOLD: needs more than one check_2_manifold_() loop, none of them has start_e == end_e.
//  VTOPO_NON_MANIFOLD_1RING: needs more than one check_2_manifold_() loop, at least one of them has start_e == end_e.
//
VTOPO_TYPE bmsh3d_vertex::detect_vtopo_type() const
{
  //1) If there is no incident edge, return VTOPO_ISOLATED.
  if (! has_incident_Es())
    return VTOPO_ISOLATED;

  //2) Reset all incident edges to be unvisited.
  unsigned int countE = 0;
  for (bmsh3d_ptr_node* cur = E_list_; cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    E->set_i_visited(0);
    countE++;
  }

  //3) Initialize firstE.
  const bmsh3d_halfedge* firstHE = get_1st_bnd_HE();
  const bmsh3d_edge* firstE;
  if (firstHE)
    firstE = firstHE->edge();
  else
    firstE = get_1st_incident_E();

  //3) Do the first check_2_manifold_() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = check_2_manifold_(firstE, cond);

  if (cond == VTOPO_EDGE_ONLY)
    return VTOPO_EDGE_ONLY;
  else if (cond == VTOPO_EDGE_JUNCTION)
    return VTOPO_EDGE_JUNCTION;
  else if (nE == countE) {
    if (cond == VTOPO_2_MANIFOLD_1RING)
      return VTOPO_2_MANIFOLD_1RING;
    else
      return VTOPO_2_MANIFOLD;
  }

  bool b_one_ring = false; //A flag to remember the first result.
  if (cond == VTOPO_2_MANIFOLD_1RING)
    b_one_ring = true;

  //4) Remaining: VTOPO_NON_MANIFOLD and VTOPO_NON_MANIFOLD_1RING
  //   Run check_2_manifold_() on all remaining unvisited edges.
  const bmsh3d_edge* nextE = find_unvisited_E_();
  while (nextE != nullptr) {
    check_2_manifold_(nextE, cond);

    if (cond == VTOPO_EDGE_ONLY)
      return VTOPO_EDGE_ONLY;
    else if (cond == VTOPO_EDGE_JUNCTION)
      return VTOPO_EDGE_JUNCTION;
    else if (cond == VTOPO_2_MANIFOLD_1RING)
      return VTOPO_NON_MANIFOLD_1RING;

    nextE = find_unvisited_E_();
  }

  if (b_one_ring)
    return VTOPO_NON_MANIFOLD_1RING;
  else
    return VTOPO_NON_MANIFOLD;
}

//: Run the 2-manifold checking loop.
//  return VTOPO_EDGE_ONLY if any encounter edge has no incident halfedge.
//  return VTOPO_EDGE_JUNCTION if any encounter edge has more than 2 incident halfedges.
//  return VTOPO_2_MANIFOLD_1RING if E goes back to startE.
//  otherwise return BOGUS_VTOPO_TYPE
//
unsigned int bmsh3d_vertex::check_2_manifold_(const bmsh3d_edge* startE,
                                              VTOPO_TYPE& cond) const
{
  unsigned int nE = 0;
  const auto* E = (const bmsh3d_edge*) startE;
  bmsh3d_halfedge* startHE = E->halfedge();
  if (startHE == nullptr) {
    cond = VTOPO_EDGE_ONLY;
    return nE;
  }

  bmsh3d_halfedge* HE = startHE;
  do {
    HE->edge()->set_i_visited(1); //During the checking, mark visited edges.
    nE++;
    if (HE->pair() && HE->pair()->pair() != HE) {
      cond = VTOPO_EDGE_JUNCTION;
      return nE;
    }

    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE(this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == nullptr) { //hit the boundary
      cond = BOGUS_VTOPO_TYPE;
      return nE+1;
    }

    HE = nextHE;
  }
  while (HE->edge() != startHE->edge());

  //hit back to startE again.
  cond = VTOPO_2_MANIFOLD_1RING;
  return nE;
}

const bmsh3d_edge* bmsh3d_vertex::find_unvisited_E_() const
{
  for (bmsh3d_ptr_node* cur = E_list_; cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (! E->b_visited())
      return E;
  }
  return nullptr;
}

void bmsh3d_vertex::getInfo(std::ostringstream& ostrm)
{
  ostrm << "\n==============================\nbmsh3d_vertex id: " << id_
        << " (" << this->pt().x()
        << ", " << this->pt().y()
        << ", " << this->pt().z()
        << ")\n  topology type: ";
  VTOPO_TYPE type = detect_vtopo_type();
  switch (type) {
   case BOGUS_VTOPO_TYPE:        ostrm << "BOGUS_VTOPO_TYPE"; break;
   case VTOPO_ISOLATED:          ostrm << "VTOPO_ISOLATED"; break;
   case VTOPO_EDGE_ONLY:         ostrm << "VTOPO_EDGE_ONLY"; break;
   case VTOPO_EDGE_JUNCTION:     ostrm << "VTOPO_EDGE_JUNCTION"; break;
   case VTOPO_2_MANIFOLD:        ostrm << "VTOPO_2_MANIFOLD"; break;
   case VTOPO_2_MANIFOLD_1RING:  ostrm << "VTOPO_2_MANIFOLD_1RING"; break;
   case VTOPO_NON_MANIFOLD:      ostrm << "VTOPO_NON_MANIFOLD"; break;
   case VTOPO_NON_MANIFOLD_1RING:ostrm << "VTOPO_NON_MANIFOLD_1RING"; break;
   default:                      ostrm << "UNDEFINED_VTOPO_TYPE"; break;
  }

  //: the incident edges
  std::set<void const*> incident_Es;
  get_incident_Es(incident_Es);
  ostrm << "\n " << incident_Es.size() << " incident edges (unordered): ";
  auto it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    auto const* E = (bmsh3d_edge const*)(*it);
    ostrm << E->id() << ' ';
  }

  //: the ordered incident faces (for 2-manifold mesh)
  std::vector<const bmsh3d_halfedge*> ordered_halfedges;
  m2_get_ordered_HEs(ordered_halfedges);

  ostrm << "\n (2-manifold) " << ordered_halfedges.size() << " ordered incident faces: ";
  for (auto HE : ordered_halfedges) {
    ostrm << ((bmsh3d_face*)HE->face())->id() << ' ';
  }

  ostrm << "\n (2-manifold) " << ordered_halfedges.size() << " ordered incident edges: ";
  for (auto HE : ordered_halfedges) {
    ostrm << HE->edge()->id() << ' ';
  }

  ostrm << std::endl;
}

const bmsh3d_halfedge* bmsh3d_vertex::get_1st_bnd_HE() const
{
  for (bmsh3d_ptr_node* cur = E_list_; cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (E->halfedge())
      if (E->halfedge()->pair() == nullptr)
        return E->halfedge();
  }
  return nullptr;
}

//: for 2-manifold mesh, return all incident halfedges (without duplicate pairs) in order return the last halfedge
bmsh3d_halfedge* bmsh3d_vertex::m2_get_ordered_HEs(std::vector<const bmsh3d_halfedge*>& ordered_halfedges) const
{
  const bmsh3d_halfedge* startHE = get_1st_bnd_HE();

  if (startHE == nullptr) { //if no boundary halfedge, just get any halfedge.
    const bmsh3d_edge* E = get_1st_incident_E();
    if (E == nullptr)
      return nullptr;
    startHE = E->halfedge();
  }

  const bmsh3d_halfedge* HE = startHE;
  do {
    ordered_halfedges.push_back(HE);
    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE(this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == nullptr)
      return otherHE; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != startHE->edge());

  return nullptr;
}

bmsh3d_halfedge* bmsh3d_vertex::m2_get_next_bnd_HE(const bmsh3d_halfedge* inputHE) const
{
  do {
    bmsh3d_halfedge* otherHE = inputHE->face()->find_other_HE(this, inputHE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == nullptr)
      return otherHE; //hit the boundary, return.

    inputHE = nextHE;
  }
  while (inputHE->edge() != inputHE->edge());

  return nullptr;
}

//:
//  for 2-manifold mesh, check if this vertex is a boundary vertex
//  or an internal vertex of the surface (mesh)
//  start tracing from inputHE to see if the loop back to inputHE
bool bmsh3d_vertex::m2_is_on_bnd(bmsh3d_halfedge* inputHE) const
{
  bmsh3d_halfedge* HE = inputHE;
  do {
    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE(this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == nullptr)
      return true; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != inputHE->edge());
  return false; //back to inputHE again.
}

//: return the sum_theta at this vertex
double bmsh3d_vertex::m2_sum_theta() const
{
  std::vector<const bmsh3d_halfedge*> ordered_halfedges;
  m2_get_ordered_HEs(ordered_halfedges);
  double sum_theta = 0;

  //loop through each adjacent face face_j.
  for (auto he_d : ordered_halfedges) {
    double theta = he_d->face()->angle_at_V(this);
    sum_theta += theta;
  }

  return sum_theta;
}

bmsh3d_edge* E_sharing_2V(const bmsh3d_vertex* V1,
                          const bmsh3d_vertex* V2)
{
  for (bmsh3d_ptr_node* cur = V1->E_list(); cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (E->is_V_incident(V2))
      return (bmsh3d_edge*)E; // casting away const!!!
  }
  return nullptr;
}

bmsh3d_face* find_F_sharing_Vs(std::vector<bmsh3d_vertex*>& vertices)
{
  bmsh3d_vertex* G = vertices[0];
  std::set<bmsh3d_face*> incident_faces;
  G->get_incident_Fs(incident_faces);

  auto it = incident_faces.begin();
  for (unsigned int i=0; i<incident_faces.size(); i++) {
    bmsh3d_face* F = (*it);
    if (F->all_Vs_incident(vertices))
      return F;
  }
  return nullptr;
}

bmsh3d_face* get_non_manifold_1ring_extra_Fs(bmsh3d_vertex* V)
{
  assert(V->has_incident_Es());

  //Reset all incident edges to be unvisited.
  const bmsh3d_edge* firstE = V->get_1st_incident_E();
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (! E->b_visited())
      E->set_i_visited(0);
  }

  //Do the first check_2_manifold_() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = V->check_2_manifold_(firstE, cond);

  assert(cond != VTOPO_EDGE_ONLY);
  assert(cond != VTOPO_EDGE_JUNCTION);
  assert(nE != V->n_incident_Es());
  if (cond != VTOPO_2_MANIFOLD_1RING)
    return firstE->halfedge()->face();

  //Remaining: VTOPO_NON_MANIFOLD and VTOPO_NON_MANIFOLD_1RING
  const bmsh3d_edge* nextE = V->find_unvisited_E_();
  return nextE->halfedge()->face();
}

//: return true if the face of e1-V-e2 is a vertex-only-incidence at V.
//  a simple check here is to check if
//    - there is at least one incident edge or face of V
//    - no V's incident edge is on vertices (V, V1) or (V, V2).
//
bool is_F_V_incidence(bmsh3d_vertex* V, const bmsh3d_vertex* V1, const bmsh3d_vertex* V2)
{
  assert(V != V1);
  assert(V != V2);
  if (! V->has_incident_Es())
    return false; //if V has no incident edges or faces, no problem.

  //Go through V's incident edges and check if any one is (V, V1) or (V, V2).
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (E->both_Vs_incident(V, V1) || E->both_Vs_incident(V, V2))
      return false;
  }
  return true;
}

const bmsh3d_edge* V_find_other_E(const bmsh3d_vertex* V, const bmsh3d_edge* inputE)
{
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != nullptr; cur = cur->next()) {
    const auto* E = (const bmsh3d_edge*) cur->ptr();
    if (E == inputE)
      continue;
    assert(E);
    return E;
  }
  return nullptr;
}
