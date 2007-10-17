#ifndef _bmsh3d_face_mc_h_
#define _bmsh3d_face_mc_h_

//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_halfedge.h
//:
// \file
// \brief Multiply Connected Face Class. A face contains inner faces.
//
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
#include <vcl_map.h>

#include "bmsh3d_face.h"
#include "bmsh3d_halfedge.h"

class bmsh3d_face_mc : public bmsh3d_face
{

protected:
  vcl_map<int, bmsh3d_halfedge*> set_he_;
  int id_counter_;

public:
  //: ====== Constructor/Destructor ======
  bmsh3d_face_mc (const int id) 
    : bmsh3d_face(id), id_counter_(0) 
  { set_he_.clear(); }

  bmsh3d_face_mc (const bmsh3d_face* face) 
    : bmsh3d_face(face->id()),  id_counter_(0) 
  { set_he_.clear(); halfedge_ = face->halfedge();}

  //: copy constructor
  //bmsh3d_face_mc (const bmsh3d_face_mc& face);

  virtual ~bmsh3d_face_mc () {}

  bmsh3d_halfedge* face_mc_map(int i);

  bool face_mc_id(bmsh3d_halfedge* he, int& i);

  int size() {return set_he_.size(); }

  vcl_map<int, bmsh3d_halfedge*> get_mc_halfedges() { return set_he_; }

  void add_mc_halfedge(bmsh3d_halfedge* he);
  
  void remove_mc_halfedge(bmsh3d_halfedge* he) {};

  void get_mc_incident_halfedges (bmsh3d_halfedge* he, 
    vcl_vector<bmsh3d_halfedge*>& incident_edges) const;

  //: given halfedge head, returns the set of incident edges 
  // of the inner face
  void get_mc_incident_edges (bmsh3d_halfedge* he, 
    vcl_vector<bmsh3d_edge*>& incident_edges) const;

  void reverse_mc_chain_of_halfedges (bmsh3d_halfedge* he);

  void print();
};

#endif
