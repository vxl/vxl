// This is brl/bbas/bgrl2/bgrl2_hg_edge.h
#ifndef bgrl2_hg_edge_h_
#define bgrl2_hg_edge_h_
//:
// \file
// \author Ming-Ching Chang
// \date   Apr 04, 2005
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli 11/15/08  Moved up to vxl
// \endverbatim

#include "bgrl2_edge.h"

#include <vcl_vector.h>
#include <vcl_cassert.h>

class bgrl2_hg_vertex;
class bgrl2_hg_hyperedge;

class bgrl2_hg_edge // : public bgrl2_edge
{
 protected:
  int id_;
  //: connecting_vertices_[0] is the starting vertex, [1] the ending vertex.
  bgrl2_hg_vertex*                connecting_vertices_[2];

  vcl_vector<bgrl2_hg_hyperedge*> connecting_hyperedges_;

 public:
  int id() const {
    return id_;
  }
  void set_id (int id) {
    id_ = id;
  }

  // ====== Graph connectivity query functions ======

  bgrl2_hg_vertex* connecting_vertices (int i) {
    return connecting_vertices_[i];
  }
  bgrl2_hg_vertex* connecting_s_vertex () {
    return connecting_vertices_[0];
  }
  bgrl2_hg_vertex* connecting_e_vertex () {
    return connecting_vertices_[1];
  }
  bgrl2_hg_vertex* get_other_connecting_vertex (bgrl2_hg_vertex* vertex) {
    if (connecting_vertices_[0] == vertex)
      return connecting_vertices_[1];
    else {
      assert (vertex == connecting_vertices_[1]);
      return connecting_vertices_[0];
    }
  }
  bool is_self_loop () const {
    return connecting_vertices_[0] == connecting_vertices_[1];
  }

  bgrl2_hg_hyperedge* connecting_hyperedges (unsigned int i) {
    assert (i<connecting_hyperedges_.size());
    return connecting_hyperedges_[i];
  }
  vcl_vector<bgrl2_hg_hyperedge*>& connecting_hyperedges() {
    return connecting_hyperedges_;
  }

  // ====== Graph connectivity modification functions ======

  void connect_vertex (int i, bgrl2_hg_vertex* vertex) {
    assert (i==0 || i==1);
    connecting_vertices_[i] = vertex;
  }
  void disconnect_vertex (int i) {
    assert (i==0 || i==1);
    connecting_vertices_[i] = NULL;
  }

  void connect_hyperedge (bgrl2_hg_hyperedge* hyperedge) {
    connecting_hyperedges_.push_back (hyperedge);
  }
  bool disconnect_hyperedge (bgrl2_hg_hyperedge* hyperedge) {
    vcl_vector<bgrl2_hg_hyperedge*>::iterator it = connecting_hyperedges_.begin();
    for (; it != connecting_hyperedges_.end(); it++) {
      bgrl2_hg_hyperedge* he = (*it);
      if (he == hyperedge) { //found it
        connecting_hyperedges_.erase (it);
        return true;
      }
    }
    assert (0);
    return false;
  }

  // ====== Constructor/Destructor and other functions ======

  bgrl2_hg_edge ()
  {
    connecting_vertices_[0] = NULL;
    connecting_vertices_[1] = NULL;
  }

  ~bgrl2_hg_edge ()
  {
  }
};

#endif // bgrl2_hg_edge_h_
