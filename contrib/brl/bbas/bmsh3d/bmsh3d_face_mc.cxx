// This is brl/bbas/bmsh3d/bmsh3d_face_mc.cxx
#include "bmsh3d_face_mc.h"
//:
// \file
// \brief Multiply Connected Face Class. A face contains inner faces.
//
// \author
//  Gamze Tunali  Dec 28, 2006
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_cassert.h>
#include <vcl_iostream.h>

#if 0 // commented out
bmsh3d_face_mc::bmsh3d_face_mc(const bmsh3d_face_mc& face)
{
  vcl_map<int, bmsh3d_halfedge*> set_he = face.set_he_;
  vcl_map<int, bmsh3d_halfedge*>::iterator it = set_he.begin();
  while (it != set_he.end()) {
    bmsh3d_halfedge* he = it->second;
    bmsh3d_edge* edge = he->edge();
    this->_add_vertex(edge->sV());
    bmsh3d_halfedge* new_he = new bmsh3d_halfedge(edge, this);
  }
}
#endif // 0

bmsh3d_halfedge* bmsh3d_face_mc::face_mc_map(int i)
{
  vcl_map<int, bmsh3d_halfedge*>::iterator it = set_he_.find (i);
  if (it == set_he_.end())
    return NULL;
  return (*it).second;
}

void bmsh3d_face_mc::add_mc_halfedge(bmsh3d_halfedge* inputHE)
{
  set_he_[id_counter_++] = inputHE;
}

bool bmsh3d_face_mc::face_mc_id (bmsh3d_halfedge* inputHE, int& id)
{
  vcl_map<int, bmsh3d_halfedge*>::iterator it = set_he_.begin();
  while (it != set_he_.end()) {
    if (inputHE == it->second) {
      id = it->first;
      return true;
    }
  }
  return false;
}

void bmsh3d_face_mc::get_mc_incident_halfedges (bmsh3d_halfedge* inputHE,
    vcl_vector<bmsh3d_halfedge*>& incident_halfedges) const
{
  // first make sure that this halfedge head is in the set
  //int id;
  //if (face_mc_id(inputHE, id) == false)
   // vcl_cerr << "Halfedge is not included in set\n";

  bmsh3d_face face(inputHE);
  face.get_incident_HEs(incident_halfedges);
}

void bmsh3d_face_mc::get_mc_incident_edges (bmsh3d_halfedge* inputHE,
                                            vcl_vector<bmsh3d_edge*>& incident_edges) const
{
#if 0
  int id;
  if (face_mc_id(inputHE, id) == false)
    vcl_cerr << "Halfedge is not included in set\n";
#endif // 0

  bmsh3d_halfedge* HE = inputHE;
  do {
    incident_edges.push_back (HE->edge());
    HE = HE->next();
  }
  while (HE != inputHE);
}

//: reverse the orientation of this inner face, i.e. reverse the chain of halfedges
void bmsh3d_face_mc::reverse_mc_chain_of_halfedges (bmsh3d_halfedge* inputHE)
{
  vcl_vector<bmsh3d_halfedge*> chain;
  bmsh3d_halfedge* HE = inputHE;
  do {
    chain.push_back (HE);
    HE = HE->next();
  }
  while (HE != inputHE);

  // build the reverse order
  HE = inputHE;
  for (int i=(int) chain.size()-1; i>=0; i--) {
    bmsh3d_halfedge* next_he = chain[i];
    HE->set_next (next_he);
    HE = next_he;
  }
  assert (HE == inputHE);
}

void bmsh3d_face_mc::print()
{
  unsigned i = 0;
  while (i < set_he_.size()) {
    bmsh3d_halfedge* startHE = set_he_[i++];
    bmsh3d_halfedge* HE = startHE;
    do {
      bmsh3d_vertex* s = HE->edge()->sV();
      bmsh3d_vertex* e = HE->edge()->eV();
      vcl_cout << "S=" << HE->edge()->sV() << ' ' << s->get_pt() << vcl_endl
               << "E=" << HE->edge()->eV() << ' ' << e->get_pt() << vcl_endl;
      HE = HE->next();
    } while (HE != startHE);
  }
}
