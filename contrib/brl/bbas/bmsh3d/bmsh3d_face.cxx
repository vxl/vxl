// This is brl/bbas/bmsh3d/bmsh3d_face.cxx
//---------------------------------------------------------------------
#include <sstream>
#include <iostream>
#include <cstdio>
#include "bmsh3d_face.h"
//:
// \file
// \brief 3D mesh face.
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

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>

#include "bmsh3d_triangle.h"
#include "bmsh3d_mesh.h"

//###############################################################
//###### Connectivity Query Functions ######
//###############################################################

void bmsh3d_face::get_incident_HEs (std::vector<bmsh3d_halfedge*>& incident_HEs) const
{
  bmsh3d_halfedge* HE = halfedge_;
  if (HE == nullptr)
    return;
  do {
    assert (HE != nullptr);
    incident_HEs.push_back (HE);
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != nullptr);
}

void bmsh3d_face::get_incident_Es (std::vector<bmsh3d_edge*>& incident_Es) const
{
  bmsh3d_halfedge* HE = halfedge_;
  do {
    incident_Es.push_back (HE->edge());
    HE = HE->next();
  }
  while (HE != halfedge_);
}

unsigned int bmsh3d_face::n_incident_Es () const
{
  unsigned int count = 0;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    count++;
    HE = HE->next();
  }
  while (HE != halfedge_ && HE != nullptr);
  return count;
}

bool bmsh3d_face::is_E_incident (const bmsh3d_edge* inputE) const
{
  bmsh3d_halfedge* HE = halfedge_;
  // If the next is NULL, it is a loop curve.
  if (HE->next() == nullptr) {
    if (HE->edge() == inputE)
      return true;
    else
      return false;
  }
  // Traverse through the circular list of halfedges,
  // and find the vertex incident with both HE and nextHE
  do {
    if (HE->edge() == inputE)
      return true;
    HE = HE->next();
  }
  while (HE != halfedge_);
  return false;
}

bmsh3d_halfedge* bmsh3d_face::find_bnd_HE () const
{
  bmsh3d_halfedge* HE = halfedge_;
  if (HE->next() == nullptr) {
    if (HE->pair() == nullptr)
      return HE;
    else
      return nullptr;
  }
  do { // Traverse through the circular list of halfedges.
    if (HE->pair() == nullptr)
      return HE;
    HE = HE->next();
  }
  while (HE != halfedge_);
  return nullptr;
}

bool bmsh3d_face::is_V_incident_via_HE (const bmsh3d_vertex* inputV) const
{
  bmsh3d_halfedge* HE = halfedge_;
  // if the next is NULL, it is a loop curve.
  // this will not happen for the fullshock mesh.
  if (HE->next() == nullptr) {
    if (HE->edge()->sV() == inputV || HE->edge()->eV() == inputV)
      return true;
    else
      return false;
  }
  do { // traverse through the circular list of halfedges
    if (HE->edge()->sV() == inputV || HE->edge()->eV() == inputV )
      return true;
    HE = HE->next();
  }
  while (HE != halfedge_);
  return false;
}

//: loop through the halfedges and locate the inputV
bmsh3d_vertex* bmsh3d_face::get_next_V_via_HE (const bmsh3d_vertex* inputV) const
{
  bmsh3d_halfedge* HE = halfedge_;
  // if the next is NULL, it is a loop curve.
  if (HE->next() == nullptr)
    return nullptr;
  do { // traverse through the circular list of halfedges,
    bmsh3d_halfedge* nextHE = HE->next();
    // find the vertex incident with both HE and nextHE
    bmsh3d_vertex* vertex = incident_V_of_Es (HE->edge(), nextHE->edge());
    if (vertex == inputV)
      return nextHE->edge()->other_V (vertex);
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return nullptr;
}

//: Given a vertex V and an edge of this face incident to V, find the other edge of this face incident to V.
bmsh3d_edge* bmsh3d_face::find_other_E (const bmsh3d_vertex* inputV,
                                        const bmsh3d_edge* inputE) const
{
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_edge* E = HE->edge();
    if (E != inputE && E->is_V_incident(inputV))
      return E;
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return nullptr;
}

//: Given a vertex V and a halfedge of this face incident to V, find the other halfedge of this face incident of V.
bmsh3d_halfedge* bmsh3d_face::find_other_HE (const bmsh3d_vertex* inputV,
                                             const bmsh3d_halfedge* inputHE) const
{
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_edge* E = HE->edge();
    if (HE != inputHE && E->is_V_incident(inputV))
      return HE;
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return nullptr;
}

//: Given a vertex V and an edge of this face incident to V, find the next edge (following the circular halfedge list) of this face incident to V.
bmsh3d_edge* bmsh3d_face::find_next_E (const bmsh3d_vertex* inputV,
                                       const bmsh3d_edge* inputE) const
{
  if (halfedge_ == nullptr)
    return nullptr;
  if (halfedge_->next() == nullptr)
    return nullptr;

  // traverse through the circular list of halfedges
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_edge* E = HE->edge();
    bmsh3d_edge* nextE = nextHE->edge();
    if (E == inputE) {
      if (nextE->is_V_incident (inputV))
        return nextE;
    }
    else if (nextE == inputE) {
      if (E->is_V_incident (inputV))
        return E;
    }
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return nullptr;
}

//: Given a vertex V and a halfedge of this face incident to V, find the next halfedge (following the circular halfedge list) of this face incident to V.
//  Note that there can exist multiple answers if inputV is multiply (>2) connected.
//  This function returns the first qualify halfedge.
bmsh3d_halfedge* bmsh3d_face::find_next_HE(const bmsh3d_vertex* inputV,
                                           const bmsh3d_halfedge* inputHE) const
{
  if (halfedge_ == nullptr)
    return nullptr;
  if (halfedge_->next() == nullptr)
    return nullptr;

  // traverse through the circular list of halfedges
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_edge* E = HE->edge();
    bmsh3d_edge* nextE = nextHE->edge();
    if (HE == inputHE) {
      if (nextE->is_V_incident (inputV))
        return nextHE;
    }
    else if (nextHE == inputHE) {
      if (E->is_V_incident (inputV))
        return HE;
    }
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (0);
  return nullptr;
}

//: get the two edges incident at this vertex and compute their angle
double bmsh3d_face::angle_at_V (const bmsh3d_vertex* inputV) const
{
  bmsh3d_edge* E1 = nullptr;
  bmsh3d_edge* E2 = nullptr;

  // Get the two edges of the face incident at the inputV
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_edge* E = HE->edge();
    if (E->is_V_incident (inputV)) {
      if (E1 == nullptr)
        E1 = E;
      else if (E2 == nullptr) {
        E2 = E;
      }
      else
        assert (0);
    }
    HE = HE->next();
  }
  while (HE != halfedge_);

  bmsh3d_vertex* v1 = E1->other_V (inputV);
  bmsh3d_vertex* v2 = E2->other_V (inputV);
  double a = vgl_distance (v1->pt(), v2->pt());
  double b = E1->length();
  double c = E2->length();
  return std::acos ( (b*b + c*c - a*a)/(b*c*2) );
}

//: Return true if this face is incident to all given vertices.
int bmsh3d_face::n_incident_Vs_in_set (std::set<bmsh3d_vertex*>& vertices) const
{
  int n_inc_V = 0;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (vertices.find (V) != vertices.end())
      n_inc_V++;
    HE = nextHE;
  }
  while (HE != halfedge_);
  return n_inc_V;
}

//: Return true if this face is incident to all given vertices.
bool bmsh3d_face::all_Vs_incident (std::vector<bmsh3d_vertex*>& vertices) const
{
  // Put all input vertices into the VSet.
  std::set<bmsh3d_vertex*> VSet;
  for (auto vertice : vertices)
    VSet.insert (vertice);
  assert (VSet.size() == vertices.size());

  // Go through the halfedge loop and remove incident vertices from VSet.
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_edge* E = HE->edge();
    VSet.erase (E->vertices(0));
    VSet.erase (E->vertices(1));
    HE = HE->next();
  }
  while (HE != halfedge_);

  return VSet.empty();
}

void bmsh3d_face::get_ordered_Vs (std::vector<bmsh3d_vertex*>& vertices) const
{
  assert (vertices.size()==0);
  if (vertices_.size() != 0)
    _get_ordered_Vs_IFS(vertices);
  else
    _get_ordered_Vs_MHE(vertices);
  assert(vertices.size() > 2);
}

void bmsh3d_face::_get_ordered_Vs_MHE(std::vector<bmsh3d_vertex*>& vertices) const
{
  bmsh3d_halfedge* HE = halfedge_;
  assert (HE->next());
  // Traverse through the circular list of halfedges,
  // and find the vertex incident with both HE and nextHE
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = incident_V_of_Es (HE->edge(), nextHE->edge());
    vertices.push_back (V);
    HE = HE->next();
  }
  while (HE != halfedge_);
}

void bmsh3d_face::_get_ordered_Vs_IFS(std::vector<bmsh3d_vertex*>& vertices) const
{
  for (auto vertice : vertices_)
    vertices.push_back (vertice);
}

void bmsh3d_face::get_ordered_V_ids (std::vector<int>& vids) const
{
  assert (vids.size()==0);
  if (vertices_.size() != 0)
    _get_ordered_V_ids_IFS(vids);
  else
    _get_ordered_V_ids_MHE(vids);
  assert (vids.size() > 2);
}

void bmsh3d_face::_get_ordered_V_ids_MHE(std::vector<int>& vids) const
{
  bmsh3d_halfedge* HE = halfedge_;
  assert (HE->next());
  // Traverse through the circular list of halfedges,
  // and find the vertex incident with both HE and nextHE
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = incident_V_of_Es (HE->edge(), nextHE->edge());
    vids.push_back (V->id());
    HE = HE->next();
  }
  while (HE != halfedge_);
}

void bmsh3d_face::_get_ordered_V_ids_IFS(std::vector<int>& vids) const
{
  for (auto vertice : vertices_)
    vids.push_back (vertice->id());
}

//###############################################################
//###### Handle local list of incident vertices ######
//###############################################################

//: track IFS ordered vertices use the halfedge data structure.
//  Store result in the vertices[] vector.
void bmsh3d_face::_ifs_track_ordered_vertices()
{
  // skip if the vertices_[] is non-empty.
  if (vertices_.size() != 0)
    return;
  // the starting halfedge is the face's pointing halfedge
  bmsh3d_halfedge* HE = halfedge_;
  // if the next is NULL, it is a loop curve.
  //  this will not happen for the fullshock mesh.
  if (HE->next() == nullptr) {
    vertices_.push_back (HE->edge()->eV());
    return;
  }
  // traverse through the circular list of halfedges,
  // and find the vertex incident with both HE and nextHE
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = incident_V_of_Es (HE->edge(), nextHE->edge());
    vertices_.push_back (V);
    HE = HE->next();
  }
  while (HE != halfedge_);
  assert (vertices_.size() > 2);
}

// Test if the face's IFS structure is correct (repeated or wrong Vids).
bool bmsh3d_face::_is_ifs_valid(bmsh3d_mesh* M)
{
  // Check if there is repeated vertices.
  std::set<bmsh3d_vertex*> Vset;
  for (auto V : vertices_) {
    Vset.insert (V);
  }
  if (Vset.size() != vertices_.size())
    return false;

  // Check if each V is inside M.
  for (auto V : vertices_) {
    if (! M->contains_V (V->id()))
      return false;
  }
  return true;
}

bool bmsh3d_face::_ifs_inside_box(const vgl_box_3d<double>& box) const
{
  for (auto V : vertices_) {
    if (!box.contains (V->pt()))
      return false;
  }
  return true;
}

bool bmsh3d_face::_ifs_outside_box(const vgl_box_3d<double>& box) const
{
  for (auto V : vertices_) {
    if (box.contains (V->pt()))
      return false;
  }
  return true;
}

//###############################################################
//###### Geometry Query Functions ######
//###############################################################

//: if any vertex out of the box return false, otherwise return true.
//  Use the halfedge_ to traverse the face.
//
bool bmsh3d_face::is_inside_box (const vgl_box_3d<double>& box) const
{
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (!box.contains (V->pt()))
        return false;
    HE = nextHE;
  }
  while (HE != halfedge_);
  return true;
}

//: if any vertex inside box return false, otherwise return true.
//  Use the halfedge_ to traverse the face.
//
bool bmsh3d_face::is_outside_box (const vgl_box_3d<double>& box) const
{
  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_halfedge* nextHE = HE->next();
    bmsh3d_vertex* V = Es_sharing_V (HE->edge(), nextHE->edge());
    if (box.contains (V->pt()))
        return false;
    HE = nextHE;
  }
  while (HE != halfedge_);
  return true;
}

vgl_point_3d<double> bmsh3d_face::compute_center_pt () const
{
  std::vector<bmsh3d_vertex*> vertices;
  get_ordered_Vs (vertices);
  return compute_cen (vertices);
}

vgl_point_3d<double> bmsh3d_face::compute_center_pt (const std::vector<bmsh3d_vertex*>& vertices) const
{
  return compute_cen (vertices);
}


//: Compute face normal using the order of halfedges
//  This function works for both convex and non-convex faces.
vgl_vector_3d<double> bmsh3d_face::compute_normal()
{
  std::vector<bmsh3d_edge*> inc_edges;
  this->get_incident_Es (inc_edges);
  vgl_point_3d<double> centroid = this->compute_center_pt();
  vgl_vector_3d<double> normal(0.0, 0.0, 0.0);

  for (auto E : inc_edges) {
    bmsh3d_halfedge* HE = E->get_HE_of_F (this);
    auto* sV = (bmsh3d_vertex*)HE->s_vertex();
    auto* eV = (bmsh3d_vertex*)HE->e_vertex();
    vgl_point_3d<double> p0 = sV->pt();
    vgl_point_3d<double> p1 = eV->pt();
    vgl_vector_3d<double> v0 = p0 - centroid;
    vgl_vector_3d<double> v1 = p1 - centroid;
    normal += cross_product(v0,v1);
  }
  return normal;
}

//###############################################################
//###### Connectivity Modification Functions ######
//###############################################################

//: Connect a halfedge to this mesh face.
//  Note that the link list is circular, but not necessarily geometrically ordered!
//  Also be careful in the empty and starting cases.
void bmsh3d_face::_connect_HE_to_end(bmsh3d_halfedge* inputHE)
{
  if (halfedge_ == nullptr) { // 1)
    halfedge_ = inputHE;
    return;
  }
  else if (halfedge_->next() == nullptr) { // 2) Only one halfedge there
    halfedge_->set_next (inputHE);
    inputHE->set_next (halfedge_);
    return;
  }
  else { // 3) The general circular list case, add to the end!
    bmsh3d_halfedge* HE = halfedge_;
    while (HE->next() != halfedge_)
      HE = HE->next();
    HE->set_next (inputHE);
    inputHE->set_next (halfedge_);
    return;
  }
}

//: remove the input halfedge from the face's halfedge list.
//  (memory of inputHE not released)
//  return true if success.
bool bmsh3d_face::_remove_HE(bmsh3d_halfedge* inputHE)
{
  if (halfedge_ == nullptr) // 1)
    return false;
  else if (halfedge_->next() == nullptr) { // 2)
    if (halfedge_ == inputHE) {
      halfedge_ = nullptr;
      return true;
    }
    else
      return false;
  }
  else { // 3)
    bmsh3d_halfedge* HE = halfedge_;
    do {
      bmsh3d_halfedge* nextHE = HE->next();
      if (nextHE == inputHE) {
        HE->set_next (nextHE->next()); // found. remove and fix pointers.
        if (nextHE == halfedge_)
          halfedge_ = HE; // fix the case if inputHE is halfedge_
        return true;
      }
      HE = HE->next();
    }
    while (HE != halfedge_);
    return false;
  }
}

//: Create a halfedge and connect a mesh face and an edge.
void bmsh3d_face::connect_E_to_end (bmsh3d_edge* E)
{
  // The halfedge will be deleted when the sheet disconnect from the sheet.
  auto* HE = new bmsh3d_halfedge (E, this);
  // Handle the both-way connectivity of halfedge-face.
  _connect_HE_to_end(HE);
  // Handle the both-way connectivity of halfedge-edge.
  E->_connect_HE_to_end(HE);
}

//: Disconnect a face and an edge (delete the halfedge).
void bmsh3d_face::disconnect_E (bmsh3d_halfedge* HE)
{
  bmsh3d_edge* E = HE->edge();
  // Disconnect HE from this face.
  _remove_HE(HE);
  // Disconnect HE from E.
  E->_disconnect_HE(HE);
  delete HE;
}

//: Sort the incident halfedges to form a circular list
bool bmsh3d_face::_sort_HEs_circular()
{
  if (halfedge_ == nullptr)
    return false;

  // put all halfedges into a vector
  std::vector<bmsh3d_halfedge*> incident_HEs;
  get_incident_HEs (incident_HEs);

  if (incident_HEs.size() == 1)
    return false; // Skip face with only one edge.
  if (incident_HEs.size() == 2)
    return false; // Skip face with only two edges.

  // Now reset the halfedges in a correct order.
  // The current halfedge_ is still the starting halfedge.
  bmsh3d_halfedge* HE = halfedge_;
  bmsh3d_edge* E = HE->edge();
  bmsh3d_vertex* eV = E->eV();
  do {
    // Find the next halfedge incident to the eV from the vector
    bmsh3d_halfedge* nextHE = _find_next_halfedge(HE, eV, incident_HEs);
    HE->set_next (nextHE);

    HE = HE->next();
    E = HE->edge();
    eV = E->other_V (eV);
  }
  while (HE != halfedge_);
  return true;
}

//: disconnect all associated halfedges from their edges and delete them.
void bmsh3d_face::_discon_all_incident_Es ()
{
  _delete_HE_chain (halfedge_);
}

//: reverse the orientation of chain of halfedges of this face.
void bmsh3d_face::_reverse_HE_chain ()
{
  if (halfedge_ == nullptr)
    return;
  if (halfedge_->next() == nullptr)
    return;

  std::vector<bmsh3d_halfedge*> HE_chain;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    HE_chain.push_back (HE);
    HE = HE->next();
  }
  while (HE != halfedge_);

  // Build the circular list of halfedges in reverse order.
  HE = halfedge_;
  for (int i=(int) HE_chain.size()-1; i>=0; i--) {
    bmsh3d_halfedge* nextHE = HE_chain[i];
    HE->set_next (nextHE);
    HE = nextHE;
  }
  assert (HE == halfedge_);
}

void bmsh3d_face::set_orientation(bmsh3d_halfedge* new_start_he,
                                  bmsh3d_vertex*   new_next_v)
{
  if (halfedge_ == nullptr)
    return;
  if (halfedge_->next() == nullptr)
    return;

  // set the new_start_he (should check)
  halfedge_ = new_start_he;

  bmsh3d_halfedge* nextHE = new_start_he->next();
  if (!nextHE->edge()->is_V_incident (new_next_v))
    _reverse_HE_chain ();
}

//###############################################################
//###### Other functions ######
//###############################################################

void bmsh3d_face::getInfo (std::ostringstream& ostrm)
{
  char s[1024];

  bmsh3d_halfedge* HE = halfedge_;
  bmsh3d_edge* e0 = HE->edge();
  HE = HE->next();
  bmsh3d_edge* e1 = HE->edge();
  HE = HE->next();
  bmsh3d_edge* e2 = HE->edge();
  unsigned int c0 = e0->n_incident_Fs();
  unsigned int c1 = e1->n_incident_Fs();
  unsigned int c2 = e2->n_incident_Fs();

  std::sprintf (s, "\n==============================\n"); ostrm<<s;
  std::sprintf (s, "bmsh3d_face id: %d, type: %s (%u-%u-%u)\n",
               id_, tri_get_topo_string().c_str(), c0, c1, c2); ostrm<<s;

  // the incident edges via halfedges
  int n_sides = n_incident_Es ();
  std::sprintf (s, " %d incident edges in order: ", n_sides); ostrm<<s;

  if (halfedge_ == nullptr) {
    std::sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->next() == nullptr) {
    std::sprintf (s, "%d ", (halfedge_)->edge()->id()); ostrm<<s;
  }
  else {
    bmsh3d_halfedge* HE = halfedge_;
    do {
      std::sprintf (s, "%d ", HE->edge()->id()); ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_);
  }

  // the incident vertices in order
  std::sprintf (s, "\n %d incident vertices in order: ", n_sides); ostrm<<s;
  if (halfedge_ == nullptr) {
    std::sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->next() == nullptr) {
    bmsh3d_halfedge* HE = halfedge_;
    assert (HE->edge()->sV() == HE->edge()->eV());
    std::sprintf (s, "%d ", HE->edge()->sV()->id()); ostrm<<s;
  }
  else {
    bmsh3d_halfedge* HE = halfedge_;
    do {
      bmsh3d_halfedge* nextHE = HE->next();
      bmsh3d_vertex* V = incident_V_of_Es (HE->edge(), nextHE->edge());

      std::sprintf (s, "%d ", V->id()); ostrm<<s;
      HE = HE->next();
    }
    while (HE != halfedge_);
  }
}

//###############################################################
//###### For triangular face only ######
//###############################################################

TRIFACE_TYPE bmsh3d_face::tri_get_topo_type () const
{
  TRIFACE_TYPE type = BOGUS_TRIFACE;

  bmsh3d_halfedge* HE = halfedge_;
  bmsh3d_edge* e0 = HE->edge();
  HE = HE->next();
  bmsh3d_edge* e1 = HE->edge();
  HE = HE->next();
  bmsh3d_edge* e2 = HE->edge();

  // Non-triangular face: more than 3 edges.
  HE = HE->next();
  if (HE->edge() != e0)
    return TRIFACE_E4P;

  unsigned int c0 = e0->n_incident_Fs();
  unsigned int c1 = e1->n_incident_Fs();
  unsigned int c2 = e2->n_incident_Fs();

  if (c0 == 1) {
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_111;
      else if (c2 == 2)
        return TRIFACE_112;
      else { assert (c2 > 2);
        return TRIFACE_113P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_112;
      else if (c2 == 2)
        return TRIFACE_122;
      else { assert (c2 > 2);
        return TRIFACE_123P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_111;
      else if (c2 == 2)
        return TRIFACE_112;
      else { assert (c2 > 2);
        return TRIFACE_113P; }
    }
  }
  else if (c0 == 2) {
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_112;
      else if (c2 == 2)
        return TRIFACE_122;
      else { assert (c2 > 2);
        return TRIFACE_123P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_122;
      else if (c2 == 2)
        return TRIFACE_222;
      else { assert (c2 > 2);
        return TRIFACE_223P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_123P;
      else if (c2 == 2)
        return TRIFACE_223P;
      else { assert (c2 > 2);
        return TRIFACE_23P3P; }
    }
  }
  else { assert (c0 > 2);
    if (c1 == 1) {
      if (c2 == 1)
        return TRIFACE_113P;
      else if (c2 == 2)
        return TRIFACE_123P;
      else { assert (c2 > 2);
        return TRIFACE_13P3P; }
    }
    else if (c1 == 2) {
      if (c2 == 1)
        return TRIFACE_123P;
      else if (c2 == 2)
        return TRIFACE_223P;
      else { assert (c2 > 2);
        return TRIFACE_23P3P; }
    }
    else { assert (c1 > 2);
      if (c2 == 1)
        return TRIFACE_13P3P;
      else if (c2 == 2)
        return TRIFACE_123P;
      else { assert (c2 > 2);
        return TRIFACE_3P3P3P; }
    }
  }

  return type;
}

std::string bmsh3d_face::tri_get_topo_string () const
{
  TRIFACE_TYPE type = tri_get_topo_type();
  switch (type) {
   case BOGUS_TRIFACE:  return "BOGUS_TRIFACE";
   case TRIFACE_111:    return "1_1_1";
   case TRIFACE_112:    return "1_1_2";
   case TRIFACE_113P:   return "1_1_3+";
   case TRIFACE_122:    return "1_2_2";
   case TRIFACE_123P:   return "1_2_3+";
   case TRIFACE_13P3P:  return "1_3+_3+";
   case TRIFACE_222:    return "2_2_2";
   case TRIFACE_223P:   return "2_2_3+";
   case TRIFACE_23P3P:  return "2_3+_3+";
   case TRIFACE_3P3P3P: return "3+_3+_3+";
   case TRIFACE_E4P:    return "edges_4+";
   default: assert(0);  return "";
  }
}

//###############################################################
//###### For the face of a 2-manifold triangular mesh only ######
//###############################################################

bmsh3d_edge* bmsh3d_face::m2t_edge_against_vertex (bmsh3d_vertex* inputV)
{
  // loop through all incident edges, look for the one sharing inputV
  bmsh3d_halfedge* HE = halfedge_;

  // traverse through the circular list of halfedges,
  do {
    bmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV))
      return E;

    HE = HE->next();
  }
  while (HE != halfedge_);

  return nullptr;
}

bmsh3d_halfedge* bmsh3d_face::m2t_halfedge_against_vertex (bmsh3d_vertex* inputV)
{
  // loop through all incident edges, look for the one sharing inputV
  bmsh3d_halfedge* HE = halfedge_;

  // traverse through the circular list of halfedges,
  do {
    bmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV))
      return HE;

    HE = HE->next();
  }
  while (HE != halfedge_);

  return nullptr;
}

//: for 2-manifold mesh, given input_face, find the neighboring face sharing the inputV.
//  It is possible that nothing could be found, if the inputV is not correct.
bmsh3d_face* bmsh3d_face::m2t_nbr_face_against_vertex (bmsh3d_vertex* inputV)
{
  // loop through all incident edges, look for the one sharing inputV
  bmsh3d_halfedge* HE = halfedge_;

  // traverse through the circular list of halfedges,
  do {
    bmsh3d_edge* E = HE->edge();
    if (!E->is_V_incident (inputV)) {
      //  found it, return the other face
      //  traverse through edge's list of halfedges.
      //  Only work for 2-manifold!!
      if (E->halfedge()->face() != this)
        return E->halfedge()->face();
      else if (E->halfedge()->pair()) {
        bmsh3d_halfedge* HE = E->halfedge()->pair();
        assert (HE->face() != this);
        return HE->face();
      }
      else {
        return nullptr;
      }
    }
    HE = HE->next();
  }
  while (HE != halfedge_);

  return nullptr;
}

//: (redundant) for 2-manifold mesh, given input_face, find the neighboring face sharing incident v1 and v2
bmsh3d_face* bmsh3d_face::m2t_nbr_face_sharing_edge (bmsh3d_vertex* v1, bmsh3d_vertex* v2)
{
  // loop through all incident edges, look for the one sharing inputV
  bmsh3d_halfedge* HE = halfedge_;

  // traverse through the circular list of halfedges,
  do {
    bmsh3d_edge* E = HE->edge();
    if (E->is_V_incident (v1) && E->is_V_incident (v2)) {
      //  found it, return the other face
      //  traverse through edge's list of halfedges.
      //  Only work for 2-manifold!!
      if (E->halfedge()->face() != this)
        return E->halfedge()->face();
      else if (E->halfedge()->pair()) {
        bmsh3d_halfedge* HE = E->halfedge()->pair();
        return HE->face();
      }
      else {
        return nullptr;
      }
    }
    HE = HE->next();
  }
  while (HE != halfedge_);

  return nullptr;
}

//: for triangular mesh only, given v1, v2, find v3
bmsh3d_vertex* bmsh3d_face::t_3rd_vertex (const bmsh3d_vertex* V1, const bmsh3d_vertex* V2) const
{
  // loop through all incident edges, look for the one sharing inputV
  bmsh3d_halfedge* HE = halfedge_;

  // traverse through the circular list of halfedges,
  do {
    bmsh3d_edge* E = HE->edge();
    // some duplication, check both s_v and e_v.
    if (E->sV() != V1 && E->sV() != V2)
      return E->sV();
    if (E->eV() != V1 && E->eV() != V2)
      return E->eV();
    HE = HE->next();
  }
  while (HE != halfedge_);

  assert (0);
  return nullptr;
}

bmsh3d_vertex* bmsh3d_face::t_vertex_against_edge (const bmsh3d_edge* E) const
{
  return t_3rd_vertex (E->sV(), E->eV());
}

//##########################################################
//###### Additional Functions ######
//##########################################################

bmsh3d_halfedge* _find_prev_in_HE_chain (const bmsh3d_halfedge* inputHE)
{
  auto* HE = (bmsh3d_halfedge*) inputHE; // casting away const !!!
  while (HE->next() != inputHE)
    HE = HE->next();
  return HE;
}

//: disconnect all associated halfedges from their edges from the given headHE.
void _delete_HE_chain (bmsh3d_halfedge* & headHE)
{
  if (headHE == nullptr)
    return;

  bmsh3d_halfedge* nextHE = headHE->next();
  if (nextHE) {
    do {
      bmsh3d_halfedge* he_to_remove = nextHE;
      nextHE = nextHE->next();
      bmsh3d_edge* E = he_to_remove->edge();
      E->_disconnect_HE (he_to_remove);
      delete he_to_remove;
    }
    while (nextHE != headHE);
  }

  // Delete the headHE and set to NULL.
  bmsh3d_edge* E = headHE->edge();
  E->_disconnect_HE (headHE);
  delete headHE;
  headHE = nullptr;
}

//  Return: the set of incident edges that get disconnected.
//  Also set the headHE to be NULL after calling it.
void _delete_HE_chain (bmsh3d_halfedge* & headHE,
                       std::vector<bmsh3d_edge*>& incident_edge_list)
{
  if (headHE == nullptr)
    return;

  bmsh3d_halfedge* nextHE = headHE->next();
  if (nextHE) {
    do {
      bmsh3d_halfedge* he_to_remove = nextHE;
      nextHE = nextHE->next();
      bmsh3d_edge* E = he_to_remove->edge();
      E->_disconnect_HE (he_to_remove);
      delete he_to_remove;
      incident_edge_list.push_back (E);
    }
    while (nextHE != headHE);
  }

  // Delete the headHE and set to NULL.
  bmsh3d_edge* E = headHE->edge();
  E->_disconnect_HE (headHE);
  delete headHE;
  headHE = nullptr;
  incident_edge_list.push_back (E);
}

//: Given the face, current halfedge, and current eV, find the next halfedge in the input storage.
bmsh3d_halfedge* _find_next_halfedge(bmsh3d_halfedge* input_he,
                                     bmsh3d_vertex* eV,
                                     std::vector<bmsh3d_halfedge*>& incident_HEs)
{
  // Search for the next halfedge that's not the input_he
  for (auto HE : incident_HEs) {
    bmsh3d_edge* E = HE->edge();
    // skip if HE is the input_he
    if (HE == input_he)
      continue;
    if (E->is_V_incident (eV))
      return HE;
  }
  return nullptr;
}

//: Assume the mesh face is planar and compute a 2D planar coordinate for it.
void get_2d_coord (const std::vector<bmsh3d_vertex*>& vertices, vgl_vector_3d<double>& N,
                   vgl_vector_3d<double>& AX, vgl_vector_3d<double>& AY)
{
  assert (vertices.size() > 2);
  // The first vertex A is the origin.
  bmsh3d_vertex* A = vertices[0];

  // The second vertex B identifies the x-axis.
  bmsh3d_vertex* B = vertices[1];
  // Make AB the unit vector in the 2D x-direction.
  AX = B->pt() - A->pt();
  AX = AX / AX.length();

  // Use The 3rd vertex C to identify normal N.
  bmsh3d_vertex* C = vertices[2];
  vgl_vector_3d<double> AC = C->pt() - A->pt();
  N = cross_product (AX, AC);
  N = N / N.length();

  // The unit vector in the 2D y-direction.
  AY = cross_product (N, AX);
  AY = AY / AY.length();
}

//: Return ordered set of vertices in 2D (x,y) coord.
//  Assume mesh face is planar. First identify the plane normal N = AB * AC.
//  Use the first vertex A as origin (0,0),
//  the second vertex B to identify x-axis, B(d, 0).
//  the y-axis is AY = cross (N, AB).
//
void get_2d_polygon (const std::vector<bmsh3d_vertex*>& vertices,
                     std::vector<double>& xs, std::vector<double>& ys)
{
  assert (vertices.size() > 2);
  xs.resize (vertices.size());
  ys.resize (vertices.size());

  // The first vertex A is the origin.
  bmsh3d_vertex* A = vertices[0];
  xs[0] = 0;
  ys[0] = 0;

  // The second vertex B identifies the x-axis.
  bmsh3d_vertex* B = vertices[1];
  double dAB = vgl_distance (A->pt(), B->pt());
  vgl_vector_3d<double> AB = B->pt() - A->pt();
  // Make AB the unit vector in the 2D x-direction.
  AB = AB / dAB;
  xs[1] = dAB;
  ys[1] = 0;

  // Use The 3rd vertex C to identify normal N.
  bmsh3d_vertex* C = vertices[2];
  vgl_vector_3d<double> AC = C->pt() - A->pt();
  vgl_vector_3d<double> N = cross_product (AB, AC);
  // Check that N is valid.
  // The unit vector in the 2D y-direction.
  vgl_vector_3d<double> AY = cross_product (N, AB);
  AY = AY / AY.length();

  // Loop through all other vertices.
  for (unsigned int i=2; i<vertices.size(); i++) {
    bmsh3d_vertex* C = vertices[i];
    vgl_vector_3d<double> AC = C->pt() - A->pt();
#if 0
    cx = d * cos theta = dot(AB, AC) / AB.
    cy = std::sqrt(AC^2 - cx^2)
#endif // 0
    xs[i] = dot_product(AC, AB);
    ys[i] = dot_product(AC, AY);
  }
}

//: Return the projected point in the local 2D (x,y) coord.
vgl_point_2d<double> get_2d_proj_pt (vgl_point_3d<double> P, const vgl_point_3d<double>& A,
                                     const vgl_vector_3d<double>& AX,
                                     const vgl_vector_3d<double>& AY)
{
  double x = dot_product (P - A, AX);
  double y = dot_product (P - A, AY);
  return {x, y};
}

//: determine the center point of the patch
vgl_point_3d<double> compute_cen (const std::vector<bmsh3d_vertex*>& vertices)
{
  double x=0.0, y=0.0, z=0.0;
  assert (vertices.size() != 0);

  for (auto vertice : vertices) {
    x += vertice->pt().x();
    y += vertice->pt().y();
    z += vertice->pt().z();
  }

  x /= vertices.size();
  y /= vertices.size();
  z /= vertices.size();

  return {x,y,z};
}

//: compute the normal of the face
vgl_vector_3d<double> compute_normal_ifs (const std::vector<bmsh3d_vertex*>& vertices)
{
  vgl_vector_3d<double> normal;
  // for P[0..n], compute cross product of (P[1]-P[0])*(P[2]-P[1]) ...
  assert (vertices.size() > 2);
  for (int i=0; i< ((int)vertices.size())-2; i++) {
    const bmsh3d_vertex* v0 = vertices[i];
    const bmsh3d_vertex* v1 = vertices[i+1];
    const bmsh3d_vertex* v2 = vertices[i+2];
    vgl_vector_3d<double> a = v1->pt() - v0->pt();
    vgl_vector_3d<double> b = v2->pt() - v1->pt();
    vgl_vector_3d<double> n = cross_product (a, b);
    normal += n;
  }
  return normal;
}

//: Compute face normal using the given edge and starting node.
vgl_vector_3d<double> compute_normal (const vgl_point_3d<double>& C,
                                      const bmsh3d_edge* E,
                                      const bmsh3d_vertex* startV)
{
  const bmsh3d_vertex* Es = startV;
  const bmsh3d_vertex* Ee = E->other_V (Es);
  return cross_product (Es->pt() - C, Ee->pt() - C);
}

//: Return true if vertices represent a polygon (Vs > 3) or obtuse triangle.
bool is_tri_non_acute (const std::vector<bmsh3d_vertex*>& vertices)
{
  if (vertices.size() > 3)
    return true;
  bmsh3d_vertex* V0 = vertices[0];
  bmsh3d_vertex* V1 = vertices[1];
  bmsh3d_vertex* V2 = vertices[2];
  return bmsh3d_is_tri_non_acute (V0->pt(), V1->pt(), V2->pt());
}

//: Return true if F (triangle) is (1,1,3+) or (1,3+,3+) extraneous
//  For a general polygon, return true if F has only edges of 1 or 3 incidences.
//
bool is_F_extraneous (bmsh3d_face* F)
{
  assert (F->halfedge() != nullptr);
  assert (F->halfedge()->next() != nullptr);
  bmsh3d_halfedge* HE = F->halfedge();
  do {
    bmsh3d_edge* E = HE->edge();
    int n = E->n_incident_Fs();
    if (n==2)
      return false;
    HE = HE->next();
  }
  while (HE != F->halfedge());
  return true;
}

bmsh3d_face* get_F_sharing_Es (bmsh3d_edge* E1, bmsh3d_edge* E2)
{
  // Loop through all incident faces of E1 and find the face incident to E2.
  if (E1->halfedge() == nullptr) {
    return nullptr;
  }
  else if (E1->halfedge()->pair() == nullptr) {
    bmsh3d_face* F = E1->halfedge()->face();
    if (F->is_E_incident (E2))
      return F;
  }
  else {
    bmsh3d_halfedge* HE = E1->halfedge();
    do {
      bmsh3d_face* F = HE->face();
      if (F->is_E_incident (E2))
        return F;
      HE = HE->pair();
    }
    while (HE != E1->halfedge());
  }
  return nullptr;
}

//###############################################################
#if 0 // Old code from Frederic
//###############################################################

//: compute the normalized N = V1 * V2
void _cross_product (double V1x, double V1y, double V1z,
                     double V2x, double V2y, double V2z,
                     double& Nx, double& Ny, double& Nz)
{
  // Vector product : NormTri = V1 x V2
  Nx = (V1y * V2z) - (V1z * V2y);
  Ny = (V1z * V2x) - (V1x * V2z);
  Nz = (V1x * V2y) - (V1y * V2x);
}

#define VECTOR_LENGTH_EPSILON       1E-14

void _normalize_vector (double& Vx, double& Vy, double& Vz)
{
  // Normalize the normal vector
  double dLength = std::sqrt (Vx*Vx + Vy*Vy + Vz*Vz);
  if (dLength < VECTOR_LENGTH_EPSILON) {
    vul_printf (std::cerr, "NUMERICAL ERROR in computing vector length.\n");
  }
  Vx = Vx / dLength;
  Vy = Vy / dLength;
  Vz = Vz / dLength;
}

void _cross_product_normalized (double V1x, double V1y, double V1z,
                                double V2x, double V2y, double V2z,
                                double& Nx, double& Ny, double& Nz)
{
  _cross_product (V1x, V1y, V1z, V2x, V2y, V2z, Nx, Ny, Nz);
  _normalize_vector (Nx, Ny, Nz);
}

//--------------------------------------------------------------------

//:
//    Given an ordered quadruplet (A,B,C,D) with C, the "left" gene,
//    A and B, the edge pair, and D the next one the "right", find
//    the dihedral angle between the pair of triangular faces
//    (ABC) and (BCD), N1 = BC x BA, N2 = BD x BC ,
//    that is the angle the two faces make with respect to the
//    shared edge BC.
//
//    GeneA:        on the linkElm triangle
//    edgePatchElm: GeneB, GeneC
//    GeneD:        the Gene to test
//
//  \returns the angle
double computeDotNormal (const bmsh3d_vertex* GeneC,
                         const dbsk3d_fs_patch_elm* edgePatchElm,
                         const bmsh3d_vertex* GeneD)
{
  const bmsh3d_vertex* GeneA = edgePatchElm->genes(0);
  const bmsh3d_vertex* GeneB = edgePatchElm->genes(1);

  // --- Compute oriented normal to 1st triangle: N1 = AB x AC ---
  double ABx = GeneB->pt().x() - GeneA->pt().x();
  double ABy = GeneB->pt().y() - GeneA->pt().y();
  double ABz = GeneB->pt().z() - GeneA->pt().z();

  double ACx = GeneC->pt().x() - GeneA->pt().x();
  double ACy = GeneC->pt().y() - GeneA->pt().y();
  double ACz = GeneC->pt().z() - GeneA->pt().z();

  // Vector product : BC x BA = N1
  double N1x, N1y, N1z;
  _cross_product_normalized (ABx, ABy, ABz, ACx, ACy, ACz, N1x, N1y, N1z);

  // --- Compute oriented normal to 2nd triangle: N2 = BD x BC ---
  double fBDx = GeneD->pt().x() - GeneA->pt().x();
  double fBDy = GeneD->pt().y() - GeneA->pt().y();
  double fBDz = GeneD->pt().z() - GeneA->pt().z();

  // Vector product : BD x BC = N2
  double N2x, N2y, N2z;
  _cross_product_normalized(fBDx, fBDy, fBDz, ABx, ABy, ABz, N2x, N2y, N2z);

  // scalar product of N1 and N2
  double dDotProduct = (N1x * N2x) + (N1y * N2y) + (N1z * N2z);

  // dot product of unit vector can not be greater than 1
  if (dDotProduct > 1.0)
    dDotProduct = 1.0;
  else if (dDotProduct < -1.0)
    dDotProduct = -1.0;

  return dDotProduct;
}

#endif // 0
