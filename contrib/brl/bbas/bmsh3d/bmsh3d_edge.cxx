// This is brl/bbas/bmsh3d/bmsh3d_edge.cxx
//---------------------------------------------------------------------
#include "bmsh3d_edge.h"
//:
// \file
// \brief Basic 3d edge
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
#include <vcl_cassert.h>
#include <vcl_cstdio.h>

#include <vgl/vgl_distance.h>

#include "bmsh3d_face.h"

//###############################################################
//###### Constructor/Destructor ######
//###############################################################

bmsh3d_edge::~bmsh3d_edge ()
{
  //when destruct this edge, also destruct all associated halfedges
  if (halfedge_ && halfedge_->pair() == NULL) {
    //if it has only one halfedge
    delete halfedge_;
  }
  else if (halfedge_) { //delete all associated halfedges pairs
    bmsh3d_halfedge* cur_he = halfedge_->pair();
    while (cur_he != halfedge_) {
      bmsh3d_halfedge* todel = cur_he;
      cur_he = cur_he->pair();
      delete todel;
    }
    //delete the last one
    delete halfedge_;
  }

  //remove the pointer from two ending vertices to this
  if (vertices_[0])
    vertices_[0]->del_incident_E (this);
  if (vertices_[1])
    if (vertices_[1] != vertices_[0])
      vertices_[1]->del_incident_E (this);
}

//: clones this edge, vertex and edge ids set to -1, set them to their real values later
bmsh3d_edge* bmsh3d_edge::clone()
{
  // the starting and ending vertices
  bmsh3d_vertex* s = vertices_[0];
  bmsh3d_vertex* e = vertices_[1];

  bmsh3d_vertex* new_s = new bmsh3d_vertex(s->get_pt().x(), s->get_pt().y(),
    s->get_pt().z(), -1);
  bmsh3d_vertex* new_e = new bmsh3d_vertex(e->get_pt().x(), e->get_pt().y(),
    e->get_pt().z(), -1);

  bmsh3d_edge* new_edge = new bmsh3d_edge(new_s, new_e, -1);
  return new_edge;
}

//###############################################################
//###### Connectivity query functions ######
//###############################################################

unsigned int bmsh3d_edge::n_incident_Fs () const
{
  // if there's no associated halfedge
  if (halfedge_ == NULL)
    return 0;

  // if there's only one associated halfedge (no loop)
  if (halfedge_->pair() == NULL)
    return 1;

  // the last case, the associated halfedges form a circular list
  unsigned int count = 0;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    count++;
    HE = HE->pair();
  }
  while (HE != halfedge_);

  return count;
}

bool bmsh3d_edge::is_F_incident (bmsh3d_face* F) const
{
  if (halfedge_ == NULL) {
    return false;
  }
  else if (halfedge_->pair() == NULL) {
    if (halfedge_->face() == F)
      return true;
  }
  else {
    bmsh3d_halfedge* HE = halfedge_;
    do {
      if (HE->face() == F)
        return true;
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }
  return false;
}

//: loop through all halfedges and find the one incident to F.
bmsh3d_halfedge* bmsh3d_edge::get_HE_of_F (bmsh3d_face* F) const
{
  if (halfedge_ == NULL) {
    return NULL;
  }
  else if (halfedge_->pair() == NULL) {
    if (halfedge_->face() == F)
      return halfedge_;
    else
      return NULL;
  }
  else { //Loop through the circular list of halfedges.
    bmsh3d_halfedge* HE = halfedge_;
    do {
      if (HE->face() == F)
        return HE;
      HE = HE->pair();
    }
    while (HE != halfedge_);
    return NULL;
  }
}

void bmsh3d_edge::get_incident_Fs (vcl_vector<bmsh3d_face*>& incident_faces) const
{
  //If there's no associated halfedge
  if (halfedge_ == NULL) {
    return;
  }
  //If there's only one associated halfedge (no loop)
  else if (halfedge_->pair() == NULL) {
    incident_faces.push_back (halfedge_->face());
    return;
  }
  //The last case, the associated halfedges form a circular list
  else {
    bmsh3d_halfedge* HE = halfedge_;
    do {
      incident_faces.push_back (HE->face());
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }
  return;
}

bmsh3d_face* bmsh3d_edge::incident_F_given_E (bmsh3d_edge* other_incident_E) const
{
  if (halfedge_ == NULL)
    return NULL;
  //if there's only one associated halfedge (no loop)
  if (halfedge_->pair() == NULL) {
    if (halfedge_->face()->is_E_incident (other_incident_E))
      return halfedge_->face();
    else
      return NULL;
  }
  //the last case, the associated halfedges form a circular list
  bmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->is_E_incident (other_incident_E))
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

bmsh3d_face* bmsh3d_edge::incident_F_given_V (bmsh3d_vertex* incident_V) const
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL) {
    if (halfedge_->face()->is_V_incident_via_HE (incident_V))
      return halfedge_->face();
    else
      return NULL;
  }
  //the last case, the associated halfedges form a circular list
  bmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face()->is_V_incident_via_HE (incident_V))
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//:
//  If E is 2-incident to one sheet
//  (internal to only one sheet), return the sheet.
//  Else, return NULL.
bmsh3d_face* bmsh3d_edge::is_2_incident_to_one_S () const
{
  vcl_vector<bmsh3d_face*> incident_faces;
  get_incident_Fs (incident_faces);
  if (incident_faces.size() == 2) {
    //Only handle the regular 2-incidence internal edge of a sheet.
    if (incident_faces[0] == incident_faces[1])
      return incident_faces[0];
    else
      return NULL;
  }
  else
    return NULL;
}

//: Check if E is 3-incident to one sheet.
bmsh3d_face* bmsh3d_edge::is_3_incident_to_one_S () const
{
  vcl_vector<bmsh3d_face*> incident_faces;
  get_incident_Fs (incident_faces);
  if (incident_faces.size() == 3) {
    //Only handle the regular 2-incidence internal edge of a sheet.
    if (incident_faces[0] == incident_faces[1] &&
        incident_faces[1] == incident_faces[2])
      return incident_faces[0];
    else
      return NULL;
  }
  else
    return NULL;
}

bmsh3d_face* bmsh3d_edge::other_2_manifold_F (bmsh3d_face* inputF) const
{
  if (n_incident_Fs() > 2)
    return NULL;

  bmsh3d_halfedge* HE = halfedge_;
  do {
    bmsh3d_face* F = HE->face();
    if (F != inputF)
      return F;
    HE = HE->pair();
  }
  while (HE != halfedge_ && HE != NULL);
  return NULL;
}

//:
//  given a tau, return the extrinsic coordinate
//  A --- p --- B
//  p = A + AP / AB;
vgl_point_3d<double> bmsh3d_edge::_point_from_tau (const double tau) const
{
  bmsh3d_vertex* sv = sV();
  bmsh3d_vertex* ev = eV();
  vgl_point_3d<double> a = sv->pt();
  vgl_point_3d<double> b = ev->pt();
  const vgl_vector_3d<double> vAB = b - a;
  return a + vAB * tau / vgl_distance (a, b);
}

vgl_point_3d<double> bmsh3d_edge::mid_pt() const
{
  bmsh3d_vertex* sv = vertices_[0];
  bmsh3d_vertex* ev = vertices_[1];
  return centre (sv->pt(), ev->pt());
}

const double bmsh3d_edge::length () const
{
  // assume that sV and eV is bmsh3d_vertex
  bmsh3d_vertex* sv = sV();
  bmsh3d_vertex* ev = eV();
  return vgl_distance (sv->pt(), ev->pt());
}

//###############################################################
//###### Connectivity Modification Functions ######
//###############################################################

void bmsh3d_edge::_connect_HE_to_end (bmsh3d_halfedge* inputHE)
{
  //Note that the link list is circular, but not necessarily geometrically ordered!
  if (halfedge_ == NULL) { //1)
    halfedge_ = inputHE;
    return;
  }
  else if (halfedge_->pair() == NULL) { //2)
    halfedge_->set_pair (inputHE);
    inputHE->set_pair (halfedge_);
    return;
  }
  else { //3) The general circular list case
    //Since the order of the halfedges is not important
    //Just insert it to the 2nd
    inputHE->set_pair (halfedge_->pair());
    halfedge_->set_pair (inputHE);
    return;
  }
}

//: delete one of the halfedge of this edge.
//  also fix the circular list of halfedge::pair_'s
void bmsh3d_edge::_disconnect_HE (bmsh3d_halfedge* inputHE)
{
  //first set its edge pointer to NULL (disconnect)
  inputHE->set_edge (NULL);

  //1) be careful on the only-one-inputHE case
  if (inputHE->pair() == NULL) {
    assert (halfedge_ == inputHE);
    halfedge_ = NULL;
    return;
  }
  bmsh3d_halfedge* prev_pair = inputHE->get_prev ();
  bmsh3d_halfedge* next_pair = inputHE->pair ();

  //if the one to be disconnected is the one the edge pointing to,
  //  replace the pointer with the next_pair available pointer.
  if (halfedge_ == inputHE)
    halfedge_ = next_pair;

  //2) be careful on the case that inputHE becomes the only-one halfedge.
  if (prev_pair == next_pair) {
    halfedge_->set_pair (NULL);
    inputHE->set_pair (NULL);
    return;
  }

  //3) the last case: general circular loop case
  prev_pair->set_pair (next_pair);

  // disconnect this inputHE from its pair
  inputHE->set_pair (NULL);
}

void bmsh3d_edge::disconnect_all_Fs (vcl_vector<bmsh3d_face*>& disconn_faces)
{
  //Repeatly disconnect all incident faces until finish.
  bmsh3d_halfedge* HE = halfedge_;
  while (HE != NULL) {
    bmsh3d_halfedge* pair = HE->pair();
    disconn_faces.push_back (HE->face());
    HE->face()->disconnect_E (HE);
    HE = pair;
  }
}

//###############################################################
//###### For the edge of a 2-manifold triangular mesh only ######
//###############################################################

bmsh3d_halfedge* bmsh3d_edge::m2_other_HE (bmsh3d_halfedge* inputHE)
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL)
    return NULL;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE != inputHE)
      return HE;
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//: for 2-manifold mesh only
bmsh3d_face* bmsh3d_edge::m2_other_face (bmsh3d_face* inputF)
{
  if (halfedge_ == NULL)
    return NULL;
  if (halfedge_->pair() == NULL)
    return NULL;
  bmsh3d_halfedge* HE = halfedge_;
  do {
    if (HE->face() != inputF)
      return HE->face();
    HE = HE->pair();
  }
  while (HE != halfedge_);
  return NULL;
}

//###############################################################
//###### Other functions ######
//###############################################################

void bmsh3d_edge::getInfo (vcl_ostringstream& ostrm)
{
  char s[1024];

  vcl_sprintf (s, "\n==============================\n"); ostrm<<s;
  vcl_sprintf (s, "bmsh3d_edge id: %d (vertices [%d] - [%d])\n", id_,
               vertices_[0]->id(), vertices_[1]->id()); ostrm<<s;

  // the incident faces via halfedges
  int n_halfedges = n_incident_Fs ();
  vcl_sprintf (s, " %d HEs: ", n_halfedges); ostrm<<s;

  if (halfedge_ == NULL) {
    vcl_sprintf (s, "NONE "); ostrm<<s;
  }
  else if (halfedge_->pair() == NULL) {
    vcl_sprintf (s, "%d ", halfedge_->face()->id()); ostrm<<s;
  }
  else {
    // the last case, the associated halfedges form a circular list
    bmsh3d_halfedge* HE = halfedge_;
    do {
      vcl_sprintf (s, "%d ", HE->face()->id()); ostrm<<s;
      HE = HE->pair();
    }
    while (HE != halfedge_);
  }

  vcl_sprintf (s, "\n"); ostrm<<s;
}

