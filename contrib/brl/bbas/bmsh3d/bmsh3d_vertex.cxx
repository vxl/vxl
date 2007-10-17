//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_vertex.cxx
//:
// \file
// \brief Basic 3d point sample
//
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_sstream.h>
#include <vcl_iostream.h>
#include <vul/vul_printf.h>

#include "bmsh3d_vertex.h"
#include "bmsh3d_edge.h"
#include "bmsh3d_face.h"

//: function to return all incident faces of this vertex
int bmsh3d_vertex::get_incident_Fs (vcl_set<bmsh3d_face*>& face_set) 
{
  //: loop through all incident edges and put all faces into the set.
  for (bmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    //loop through all incident halfedges of this edge and get the associated faces
    if (E->halfedge() == NULL) {
    }
    else if (E->halfedge()->pair() == NULL) {
      face_set.insert (E->halfedge()->face());
    }    
    else { //the last case, the associated halfedges form a circular list
      bmsh3d_halfedge* HE = E->halfedge();
      do {
        face_set.insert (HE->face());
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
//  VTOPO_2_MANIFOLD: one _check_2_manifold() loop to finish all traversing, start_e != end_e.
//  VTOPO_2_MANIFOLD_1RING: one _check_2_manifold() loop to finish all traversing, start_e == end_e.
//  VTOPO_NON_MANIFOLD: needs more than one _check_2_manifold() loop, none of them has start_e == end_e.
//  VTOPO_NON_MANIFOLD_1RING: needs more than one _check_2_manifold() loop, at least one of them has start_e == end_e.
//
VTOPO_TYPE bmsh3d_vertex::detect_vtopo_type () const
{  
  //1) If there is no incident edge, return VTOPO_ISOLATED.
  if (has_incident_Es() == false)
    return VTOPO_ISOLATED;

  //2) Reset all incident edges to be unvisited.
  unsigned int countE = 0;
  for (bmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    E->set_i_visited (0);
    countE++;
  }

  //3) Initialize firstE.
  const bmsh3d_halfedge* firstHE = get_1st_bnd_HE();
  bmsh3d_edge* firstE;
  if (firstHE)
    firstE = firstHE->edge();
  else
    firstE = get_1st_incident_E();

  //3) Do the first _check_2_manifold() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = _check_2_manifold (firstE, cond);

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
  //   Run _check_2_manifold() on all remaining unvisited edges.
  bmsh3d_edge* nextE = _find_unvisited_E ();
  while (nextE != NULL) {
    _check_2_manifold (nextE, cond);

    if (cond == VTOPO_EDGE_ONLY)
      return VTOPO_EDGE_ONLY;
    else if (cond == VTOPO_EDGE_JUNCTION)
      return VTOPO_EDGE_JUNCTION;
    else if (cond == VTOPO_2_MANIFOLD_1RING)
      return VTOPO_NON_MANIFOLD_1RING;

    nextE = _find_unvisited_E ();
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
unsigned int bmsh3d_vertex::_check_2_manifold (const bmsh3d_edge* startE, 
                                                VTOPO_TYPE& cond) const
{
  unsigned int nE = 0;
  bmsh3d_edge* E = (bmsh3d_edge*) startE;
  bmsh3d_halfedge* startHE = E->halfedge();
  if (startHE == NULL) {
    cond = VTOPO_EDGE_ONLY;
    return nE;
  }

  bmsh3d_halfedge* HE = startHE;
  do { 
    HE->edge()->set_i_visited (1); //During the checking, mark visited edges.
    nE++;
    if (HE->pair() && HE->pair()->pair() != HE) {
      cond = VTOPO_EDGE_JUNCTION;
      return nE;
    }

    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE (this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL) { //hit the boundary
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

bmsh3d_edge* bmsh3d_vertex::_find_unvisited_E () const 
{
  for (bmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E->b_visited() == false)
      return E;
  }
  return NULL;
}

void bmsh3d_vertex::getInfo (vcl_ostringstream& ostrm) 
{
  char s[1024];

  bmsh3d_vertex* _this = this;

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "bmsh3d_vertex id: %d (%.12f, %.12f, %.12f)\n", id_,
               _this->pt().x(), _this->pt().y(), _this->pt().z()); ostrm<<s;
  
  vcl_sprintf (s, "  topology type: "); ostrm<<s;
  VTOPO_TYPE type = detect_vtopo_type ();
  switch (type) {
  case BOGUS_VTOPO_TYPE:
    vcl_sprintf (s, "BOGUS_VTOPO_TYPE"); ostrm<<s;
  break;
  case VTOPO_ISOLATED:
    vcl_sprintf (s, "VTOPO_ISOLATED"); ostrm<<s;
  break;
  case VTOPO_EDGE_ONLY:
    vcl_sprintf (s, "VTOPO_EDGE_ONLY"); ostrm<<s;
  break;
  case VTOPO_EDGE_JUNCTION:
    vcl_sprintf (s, "VTOPO_EDGE_JUNCTION"); ostrm<<s;
  break;
  case VTOPO_2_MANIFOLD:
    vcl_sprintf (s, "VTOPO_2_MANIFOLD"); ostrm<<s;
  break;
  case VTOPO_2_MANIFOLD_1RING:
    vcl_sprintf (s, "VTOPO_2_MANIFOLD_1RING"); ostrm<<s;
  break;
  case VTOPO_NON_MANIFOLD:
    vcl_sprintf (s, "VTOPO_NON_MANIFOLD"); ostrm<<s;
  break;
  case VTOPO_NON_MANIFOLD_1RING:
    vcl_sprintf (s, "VTOPO_NON_MANIFOLD_1RING"); ostrm<<s;
  break;
  }

  //: the incident edges
  vcl_set<void*> incident_Es;
  get_incident_Es (incident_Es);
  vcl_sprintf (s, "\n %u incident edges (unordered): ", incident_Es.size()); ostrm<<s;
  vcl_set<void*>::iterator it = incident_Es.begin();
  for (; it != incident_Es.end(); it++) {
    bmsh3d_edge* E = (bmsh3d_edge*) (*it);
    vcl_sprintf (s, "%d ", E->id()); ostrm<<s;
  }

  //: the ordered incident faces (for 2-manifold mesh)
  vcl_vector<bmsh3d_halfedge*> ordered_halfedges;
  m2_get_ordered_HEs (ordered_halfedges);  

  vcl_sprintf (s, "\n (2-manifold) %d ordered incident faces: ", ordered_halfedges.size()); ostrm<<s;
  for (unsigned int i=0; i<ordered_halfedges.size(); i++) {
    bmsh3d_halfedge* HE = ordered_halfedges[i];
    vcl_sprintf (s, "%d ", ((bmsh3d_face*)HE->face())->id()); ostrm<<s;
  }
  
  vcl_sprintf (s, "\n (2-manifold) %d ordered incident edges: ", ordered_halfedges.size()); ostrm<<s;
  for (unsigned int i=0; i<ordered_halfedges.size(); i++) {
    bmsh3d_halfedge* HE = ordered_halfedges[i];
    vcl_sprintf (s, "%d ", HE->edge()->id()); ostrm<<s;
  }
  
  vcl_sprintf (s, "\n"); ostrm<<s;
}

bmsh3d_halfedge* bmsh3d_vertex::get_1st_bnd_HE () const
{
  for (bmsh3d_ptr_node* cur = E_list_; cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E->halfedge())
      if (E->halfedge()->pair() == NULL)
        return E->halfedge();
  }
  return NULL;
}

//: for 2-manifold mesh, return all incident halfedges (without duplicate pairs) in order
//  return the last halfedge
bmsh3d_halfedge* bmsh3d_vertex::m2_get_ordered_HEs (vcl_vector<bmsh3d_halfedge*>& ordered_halfedges) const
{
  bmsh3d_halfedge* startHE = get_1st_bnd_HE();
  
  if (startHE == NULL) { //if no boundary halfedge, just get any halfedge.
    bmsh3d_edge* E = (bmsh3d_edge*) get_1st_incident_E();
    if (E == NULL)
      return NULL;
    startHE = E->halfedge();
  }

  bmsh3d_halfedge* HE = startHE;
  do {
    ordered_halfedges.push_back (HE);
    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE (this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL)
      return otherHE; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != startHE->edge());

  return NULL;
}

bmsh3d_halfedge* bmsh3d_vertex::m2_get_next_bnd_HE (const bmsh3d_halfedge* inputHE) const
{
  bmsh3d_halfedge* HE = (bmsh3d_halfedge*) inputHE;
  do {
    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE (this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL)      
      return otherHE; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != inputHE->edge());

  return NULL;
}

//: for 2-manifold mesh, check if this vertex is a boundary vertex
//  or an internal vertex of the surface (mesh)
//  start tracing from inputHE to see if the loop back to inputHE
bool bmsh3d_vertex::m2_is_on_bnd (bmsh3d_halfedge* inputHE) const
{
  bmsh3d_halfedge* HE = inputHE;
  do {
    bmsh3d_halfedge* otherHE = HE->face()->find_other_HE (this, HE);
    bmsh3d_halfedge* nextHE = otherHE->pair();
    if (nextHE == NULL) 
      return true; //hit the boundary, return.

    HE = nextHE;
  }
  while (HE->edge() != inputHE->edge());  
  return false; //back to inputHE again.
}

//: return the sum_theta at this vertex
double bmsh3d_vertex::m2_sum_theta () const
{
  vcl_vector<bmsh3d_halfedge*> ordered_halfedges;
  bmsh3d_halfedge* last_he = m2_get_ordered_HEs (ordered_halfedges);
  double sum_theta = 0;

  //loop through each adjacent face face_j.
  for (unsigned int j=0; j<ordered_halfedges.size(); j++) {
    bmsh3d_halfedge* he_d = ordered_halfedges[j];
    double theta = he_d->face()->angle_at_V (this);
    sum_theta += theta;
  }

  return sum_theta;
}

bmsh3d_edge* E_sharing_2V (const bmsh3d_vertex* V1, 
                            const bmsh3d_vertex* V2)
{
  for (bmsh3d_ptr_node* cur = V1->E_list(); cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E->is_V_incident (V2))
      return E;
  }
  return NULL;
}

bmsh3d_face* find_F_sharing_Vs (vcl_vector<bmsh3d_vertex*>& vertices)
{
  bmsh3d_vertex* G = vertices[0];
  vcl_set<bmsh3d_face*> incident_faces;
  G->get_incident_Fs (incident_faces);

  vcl_set<bmsh3d_face*>::iterator it = incident_faces.begin();
  for (unsigned int i=0; i<incident_faces.size(); i++) {
    bmsh3d_face* F = (*it);
    if (F->all_Vs_incident (vertices))
      return F;
  }
  return NULL;
}

bmsh3d_face* get_non_manifold_1ring_extra_Fs (bmsh3d_vertex* V)
{
  assert (V->has_incident_Es());

  //Reset all incident edges to be unvisited.
  const bmsh3d_edge* firstE = V->get_1st_incident_E ();
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E->b_visited() == false)
      E->set_i_visited (0);
  }

  //Do the first _check_2_manifold() loop starting from startE.
  VTOPO_TYPE cond;
  unsigned int nE = V->_check_2_manifold (firstE, cond);

  assert (cond != VTOPO_EDGE_ONLY);
  assert (cond != VTOPO_EDGE_JUNCTION);
  assert (nE != V->n_incident_Es());
  if (cond != VTOPO_2_MANIFOLD_1RING)
    return firstE->halfedge()->face();

  //Remaining: VTOPO_NON_MANIFOLD and VTOPO_NON_MANIFOLD_1RING
  bmsh3d_edge* nextE = V->_find_unvisited_E ();
  return nextE->halfedge()->face();
}

//: return true if the face of e1-V-e2 is a vertex-only-incidence at V.
//  a simple check here is to check if 
//    - there is at least one incident edge or face of V
//    - no V's incident edge is on vertices (V, V1) or (V, V2).
//
bool is_F_V_incidence (bmsh3d_vertex* V, const bmsh3d_vertex* V1, const bmsh3d_vertex* V2)
{
  assert (V != V1);
  assert (V != V2);
  if (V->has_incident_Es() == false)
    return false; //if V has no incident edges or faces, no problem.

  //Go through V's incident edges and check if any one is (V, V1) or (V, V2).
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E->both_Vs_incident (V, V1) || E->both_Vs_incident (V, V2))
      return false;
  }
  return true;
}

bmsh3d_edge* V_find_other_E (const bmsh3d_vertex* V, const bmsh3d_edge* inputE)
{
  for (bmsh3d_ptr_node* cur = V->E_list(); cur != NULL; cur = cur->next()) {
    bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
    if (E == inputE)
      continue;
    assert (E);
    return E;
  }
  return NULL;
}

