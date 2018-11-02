// This is brl/bbas/bgrl2/bgrl2_hg_hyperedge.h
#ifndef bgrl2_hg_hyperedge_h_
#define bgrl2_hg_hyperedge_h_
//:
// \file
// \author Ming-Ching Chang
// \date   Apr 04, 2005
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli 11/15/08  Moved up to vxl
// \endverbatim

#include <iostream>
#include <vector>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <cassert>

class bgrl2_hg_vertex;
class bgrl2_hg_edge;

class bgrl2_hg_hyperedge
{
 protected:
  int id_;
  //: e1=(v1, v2), e2=(v2, v3), ..., en=(vn, v1)
  std::vector<bgrl2_hg_edge*>    connecting_edges_;
  std::vector<bgrl2_hg_vertex*>  connecting_vertices_;

 public:
  int id() const {
    return id_;
  }
  void set_id (int id) {
    id_ = id;
  }
  //: ====== Graph connectivity query functions ======
  bgrl2_hg_edge* connecting_edges (unsigned int i) {
    assert (i<connecting_edges_.size());
    return connecting_edges_[i];
  }
  std::vector<bgrl2_hg_edge*>& connecting_edges() {
    return connecting_edges_;
  }

  bgrl2_hg_vertex* connecting_vertices (unsigned int i) {
    assert (i<connecting_vertices_.size());
    return connecting_vertices_[i];
  }
  std::vector<bgrl2_hg_vertex*>& connecting_vertices() {
    return connecting_vertices_;
  }

  //: ====== Graph connectivity modification functions ======
  void connect_edge (bgrl2_hg_edge* edge) {
    connecting_edges_.push_back (edge);
  }
  bool disconnect_edge (bgrl2_hg_edge* edge) {
    std::vector<bgrl2_hg_edge*>::iterator it = connecting_edges_.begin();
    for (; it != connecting_edges_.end(); it++) {
      bgrl2_hg_edge* e = (*it);
      if (e == edge) { //found it
        connecting_edges_.erase (it);
        return true;
      }
    }
    assert (0);
    return false;
  }

  void connect_vertex (bgrl2_hg_vertex* vertex) {
    connecting_vertices_.push_back (vertex);
  }
  bool disconnect_vertex (bgrl2_hg_vertex* vertex) {
    std::vector<bgrl2_hg_vertex*>::iterator it = connecting_vertices_.begin();
    for (; it != connecting_vertices_.end(); it++) {
      bgrl2_hg_vertex* v = (*it);
      if (v == vertex) { //found it
        connecting_vertices_.erase (it);
        return true;
      }
    }
    assert (0);
    return false;
  }

  //: ====== Constructor/Destructor and other functions ======
  bgrl2_hg_hyperedge ()
  = default;

  ~bgrl2_hg_hyperedge ()
  = default;
};

#endif // bgrl2_hg_hyperedge_h_
