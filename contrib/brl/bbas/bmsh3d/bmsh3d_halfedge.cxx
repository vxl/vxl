// This is brl/bbas/bmsh3d/bmsh3d_halfedge.cxx
//---------------------------------------------------------------------
#include "bmsh3d_halfedge.h"
//:
// \file
// \brief Basic 3d edge
//
//
// \author
//  MingChing Chang  Apr 22, 2005
//
// \verbatim
//  Modifications
//     Jan 22, 2007 Gamze Tunali - added sV() and eV() methods
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include "bmsh3d_edge.h"
#include "bmsh3d_face.h"
#include <vcl_cassert.h>

//: warning: this vertex may be different than the he->edge's sV
bmsh3d_vertex* bmsh3d_halfedge::s_vertex() const
{
  bmsh3d_halfedge* next = this->next();
  bmsh3d_vertex* e = incident_V_of_Es (edge_, next->edge());
  bmsh3d_vertex* s = edge_->other_V (e);
  return s;
}

//: warning: this vertex may be different than the he->edge's eV
bmsh3d_vertex* bmsh3d_halfedge::e_vertex() const
{
  bmsh3d_halfedge* next = this->next();
  bmsh3d_vertex* e = incident_V_of_Es (edge_, next->edge());
  return e;
}

//: Given two consecutive edges, find the common incident vertex.
bmsh3d_vertex* incident_V_of_Es (const bmsh3d_halfedge* HE0,
                                      const bmsh3d_halfedge* HE1)
{
  bmsh3d_edge* E0 = HE0->edge();
  bmsh3d_edge* E1 = HE1->edge();

  return incident_V_of_Es (E0, E1);
}

void add_HE_to_circular_chain_end (bmsh3d_halfedge* headHE, bmsh3d_halfedge* inputHE)
{
  assert (headHE != NULL);
  if (headHE->next() == NULL) {
    headHE->set_next (inputHE);
    inputHE->set_next (headHE);
  }
  //Trace to the end of the headHE circular list.
  bmsh3d_halfedge* prevHE = headHE;
  while (prevHE->next() != headHE)
    prevHE = prevHE->next();
  //Add inputHE to the end.
  prevHE->set_next (inputHE);
  inputHE->set_next (headHE);
}


