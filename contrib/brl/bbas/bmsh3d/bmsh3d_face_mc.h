// This is brl/bbas/bmsh3d/bmsh3d_face_mc.h
//---------------------------------------------------------------------
#ifndef bmsh3d_face_mc_h_
#define bmsh3d_face_mc_h_
//:
// \file
// \brief Multiply Connected Face Class. A face contains inner faces.
//
// \author
//  Gamze Tunali  Dec 28, 2006
//
// \verbatim
//  Modifications
//   <none yet>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <iostream>
#include <map>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

#include "bmsh3d_face.h"
#include "bmsh3d_halfedge.h"
class bmsh3d_edge;

class bmsh3d_face_mc : public bmsh3d_face
{
 protected:
  std::map<int, bmsh3d_halfedge*> set_he_;
  int id_counter_;

 public:
  // ====== Constructor/Destructor ======

  bmsh3d_face_mc (const int id)
    : bmsh3d_face(id), id_counter_(0)
  { set_he_.clear(); }

  bmsh3d_face_mc (const bmsh3d_face* face)
    : bmsh3d_face(face->id()),  id_counter_(0)
  { set_he_.clear(); halfedge_ = face->halfedge();}

#if 0
  //: copy constructor
  bmsh3d_face_mc (const bmsh3d_face_mc& face);
#endif

  ~bmsh3d_face_mc () override = default;

  bmsh3d_halfedge* face_mc_map(int i);

  bool face_mc_id(bmsh3d_halfedge* he, int& i);

  int size() const {return set_he_.size(); }

  std::map<int, bmsh3d_halfedge*> get_mc_halfedges() { return set_he_; }

  void add_mc_halfedge(bmsh3d_halfedge* he);

  void remove_mc_halfedge(bmsh3d_halfedge* /*he*/) {}

  void get_mc_incident_halfedges (bmsh3d_halfedge* he,
                                  std::vector<bmsh3d_halfedge*>& incident_edges) const;

  //: given halfedge head, returns the set of incident edges of the inner face
  void get_mc_incident_edges (bmsh3d_halfedge* he,
                              std::vector<bmsh3d_edge*>& incident_edges) const;

  void reverse_mc_chain_of_halfedges (bmsh3d_halfedge* he);

  void print();
};

#endif
