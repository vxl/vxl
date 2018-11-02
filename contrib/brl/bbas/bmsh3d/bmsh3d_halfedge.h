// This is brl/bbas/bmsh3d/bmsh3d_halfedge.h
//---------------------------------------------------------------------
#ifndef bmsh3d_halfedge_h_
#define bmsh3d_halfedge_h_
//:
// \file
// \brief Basic 3d halfedge on a mesh
//
// \author
//  MingChing Chang  May 30, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include "bmsh3d_utils.h"
#include "bmsh3d_vertex.h"

#include <cassert>
#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif

//#######################################################
//     The Modified Half-Edge Data Structure
//#######################################################

class bmsh3d_edge;
class bmsh3d_face;

//: the pointer to two ending vertex are in the bmsh3d_edge class.
class bmsh3d_halfedge
{
 protected:
  //: For manifold-mesh, the pair's pair is itself
  //  For non-manifold mesh, the pairs form a circular list
  bmsh3d_halfedge*  pair_;

  //: The nexts form a circular list bounding a face
  bmsh3d_halfedge*  next_;

  bmsh3d_face* face_;

  //: pointer to the bmsh3d_edge
  bmsh3d_edge* edge_;

 public:
  //###### Constructor/Destructor ######
  bmsh3d_halfedge (bmsh3d_halfedge* pair, bmsh3d_halfedge* next,
                   bmsh3d_edge* edge, bmsh3d_face* face)
  {
    pair_ = pair;
    next_ = next;
    face_ = face;
    edge_ = edge;
  }

  bmsh3d_halfedge (bmsh3d_edge* edge, bmsh3d_face* face)
  {
    face_ = face;
    edge_ = edge;
    pair_ = nullptr;
    next_ = nullptr;
  }

  virtual ~bmsh3d_halfedge () {
    //: make sure that its pair_ is properly disconnected!
    assert (pair_ == nullptr);

    //  don't need to worry about the next_ here
    //  assume the topological consistency is handled
    //  when a face is deleted!

    //: make sure that no empty halfedge exists
    assert (face_ != nullptr);

    //  make sure that it is already disconnected from the edge
    //  You should use bmsh3d::remove_face() and
    //  bmsh3d::remove_edge() to ensure topological consistency.
    assert (edge_ == nullptr);
  }

  //###### Data access functions ######
  bmsh3d_halfedge* pair() const {
    return pair_;
  }
  void set_pair (bmsh3d_halfedge* pair) {
    pair_ = pair;
  }

  bmsh3d_halfedge* next() const {
    return next_;
  }
  void set_next (bmsh3d_halfedge* next) {
    next_ = next;
  }

  bmsh3d_face* face() const {
    return face_;
  }
  void set_face (bmsh3d_face* face) {
    face_ = face;
  }

  bmsh3d_edge* edge() const {
    return edge_;
  }
  void set_edge (bmsh3d_edge* edge) {
    edge_ = edge;
  }

  //: returns the starting vertex for this halfedge based on the direction
  bmsh3d_vertex* s_vertex() const;

  //: returns the ending vertex for this halfedge based on the direction
  bmsh3d_vertex* e_vertex() const;

  //: brute-force search for the previous halfedge pair
  const bmsh3d_halfedge* get_prev () const {
    //: be careful on the only-one-halfedge case!
    if (pair_ == nullptr)
      return nullptr;
    const bmsh3d_halfedge* HE = this;
    while (HE->pair() != this)
      HE = HE->pair();
    return HE;
  }
};

void add_HE_to_circular_chain_end (bmsh3d_halfedge* headHE, bmsh3d_halfedge* inputHE);

//: Given two consecutive edges, find the common incident vertex.
bmsh3d_vertex* incident_V_of_Es (const bmsh3d_halfedge* he0, const bmsh3d_halfedge* he1);

#endif // bmsh3d_halfedge_h_
