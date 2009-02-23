// This is brl/bbas/bgrl2/bgrl2_hg_vertex.h
#ifndef bgrl2_hg_vertex_h_
#define bgrl2_hg_vertex_h_
//:
// \file
// \author Ming-Ching Chang
// \date   Apr 04, 2005
//
// \verbatim
//  Modifications
//   Ozge C. Ozcanli 11/15/08  Moved up to vxl
// \endverbatim

#include <vcl_vector.h>
#include <vcl_cassert.h>

#include <bgrl2/bgrl2_vertex.h>

class bgrl2_hg_edge;
class bgrl2_hg_hyperedge;

class bgrl2_hg_vertex //: public bgrl2_vertex
{
 protected:
  int id_;
  vcl_vector<bgrl2_hg_edge*>      connecting_edges_;
  vcl_vector<bgrl2_hg_hyperedge*> connecting_hyperedges_;

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
  vcl_vector<bgrl2_hg_edge*>& connecting_edges() {
    return connecting_edges_;
  }

  bgrl2_hg_hyperedge* connecting_hyperedges (unsigned int i) {
    assert (i<connecting_hyperedges_.size());
    return connecting_hyperedges_[i];
  }
  vcl_vector<bgrl2_hg_hyperedge*>& connecting_hyperedges() {
    return connecting_hyperedges_;
  }

  //: ====== Graph connectivity modification functions ======
  void connect_edge (bgrl2_hg_edge* edge) {
    connecting_edges_.push_back (edge);
  }
  bool disconnect_edge (bgrl2_hg_edge* edge) {
    vcl_vector<bgrl2_hg_edge*>::iterator it = connecting_edges_.begin();
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

  //: ====== Constructor/Destructor and other functions ======
  bgrl2_hg_vertex () //: bgrl2_vertex ()
  {
  }

  ~bgrl2_hg_vertex ()
  {
  }
};

#endif // bgrl2_hg_vertex_h_
