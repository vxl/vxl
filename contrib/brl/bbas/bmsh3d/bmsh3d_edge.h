// This is brl/bbas/bmsh3d/bmsh3d_edge.h
#ifndef bmsh3d_edge_h_
#define bmsh3d_edge_h_
//---------------------------------------------------------------------
//:
// \file
// \brief Basic 3d edge on a mesh
//
// \author
//  MingChing Chang  June 13, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vector>
#include <iostream>
#include <sstream>
#include "bmsh3d_halfedge.h"
#include "bmsh3d_utils.h"

#ifdef _MSC_VER
#  include <vcl_msvc_warnings.h>
#endif
#include <vgl/vgl_fwd.h>

//#######################################################
//     The Mesh Library Data Structure
//     (Combined IndexedFaceSet and Half-Edge.
//      Use depend on what you need.)
//     The actual edge element that's under both-way pointer to the half-edges.
//     For a manifold mesh:
//          - two pointers to both half-edges.
//     For a non-manifold mesh:
//          - a vector of pointers the other half-edges.
//#######################################################

class bmsh3d_face;

//: A manifold edge element (or link) has pointer to one of the half edges
class bmsh3d_edge : public vispt_elm
{
 protected:
  //: pointer to one of the halfedges associated with it.
  //  The halfedges form a circular list.
  //  Note that we don't need to use type 'he_halfedge' here!
  bmsh3d_halfedge* halfedge_;

  //: the starting and ending vertices
  bmsh3d_vertex*   vertices_[2];

  //: all bmsh3d_vertex, bmsh3d_edge, bmsh3d_face have an id
  int               id_;

  //: The visited flag for mesh hypergraph traversal.
  //    - b_visited_:
  mutable int      i_visited_;

 public:
  //###### Constructor/Destructor ######
  bmsh3d_edge(bmsh3d_vertex* sv, bmsh3d_vertex* ev, int id)
  {
    id_           = id;
    halfedge_     = nullptr;
    vertices_[0]  = sv;
    vertices_[1]  = ev;
    i_visited_    = 0;
  }
  ~bmsh3d_edge() override;
  bmsh3d_edge* clone();

  //###### Data access functions ######

  int id() const { return id_; }
  void set_id(int id) { id_ = id; }
  bmsh3d_halfedge* halfedge() const { return halfedge_; }
  void set_halfedge(bmsh3d_halfedge*  halfedge) { halfedge_ = halfedge; }
  bool b_visited() const { return i_visited_ != 0; }
  void set_visited(bool b) const { i_visited_ = b ? 1 : 0; } /* mutable */

  //: if i_visited_ less than i_traverse_flag, it's not visited
  bool is_visited(int traverse_value) const { return i_visited_ >= traverse_value; }
  void set_i_visited(int traverse_value) const { i_visited_ = traverse_value; }

  bool is_valid() const { return i_visited_ != 0; }
  void set_valid(bool v) const {  i_visited_ = v ? 1 : 0; } /* mutable */

  //###### Connectivity query functions ######
  bmsh3d_vertex* sV() const { return vertices_[0]; }
  bmsh3d_vertex* eV() const { return vertices_[1]; }
  bmsh3d_vertex* vertices(unsigned int i) const { return vertices_[i]; }
  void set_vertex(unsigned int i, bmsh3d_vertex* V) { vertices_[i] = V; }
  bmsh3d_vertex* other_V(const bmsh3d_vertex* V) const {
    if (vertices_[0] == V)
      return vertices_[1];

    if (vertices_[1] == V)
      return vertices_[0];

    return nullptr;
  }
  bool is_V_incident(const bmsh3d_vertex* V) const {
    return vertices_[0] == V || vertices_[1] == V;
  }
  bool both_Vs_incident(const bmsh3d_vertex* V0, const bmsh3d_vertex* V1) const {
    return (vertices_[0] == V0 && vertices_[1] == V1)
        || (vertices_[0] == V1 && vertices_[1] == V0);
  }
  bool is_self_loop() const { return vertices_[0] == vertices_[1]; }

  unsigned int n_incident_Fs() const;
  bool is_F_incident(const bmsh3d_face* F) const;
  bool is_F_incident(bmsh3d_face* F) const;
  bmsh3d_halfedge* get_HE_of_F(bmsh3d_face* F) const;

  void get_incident_Fs(std::vector<bmsh3d_face*>& incident_faces) const;

  bmsh3d_face* incident_F_given_E(bmsh3d_edge* other_incident_E) const;
  bmsh3d_face* incident_F_given_V(bmsh3d_vertex* incident_vertex) const;

  //: Check if E is 2-incident to one sheet.
  bmsh3d_face* is_2_incident_to_one_S() const;
  //: Check if E is 3-incident to one sheet.
  bmsh3d_face* is_3_incident_to_one_S() const;

  bmsh3d_face* other_2_manifold_F(bmsh3d_face* inputF) const;

  //: given a tau, return the extrinsic coordinate
  vgl_point_3d<double> _point_from_tau(const double tau) const;

  //: mid-point of this link
  vgl_point_3d<double> mid_pt() const;

  //: length of this link.
  double length() const;

  //###### Connectivity Modification Functions ######
  void switch_s_e_vertex() {
    bmsh3d_vertex* temp = vertices_[0];
    vertices_[0] = vertices_[1];
    vertices_[1] = temp;
  }

  void _connect_HE_to_end(bmsh3d_halfedge* inputHE);

  //: Disconnect one of the halfedge of this edge and fix the circular list of halfedge pairs.
  void _disconnect_HE(bmsh3d_halfedge* inputHE);

  //: disconnect all incident faces and return the vector of all such faces.
  void disconnect_all_Fs(std::vector<bmsh3d_face*>& disconn_faces);

  //###### For the edge of a 2-manifold triangular mesh only ######
  bmsh3d_halfedge* m2_other_HE(bmsh3d_halfedge* inputHE);
  bmsh3d_face* m2_other_face(bmsh3d_face* input_face);

  //###### Other functions ######
  void getInfo(std::ostringstream& ostrm) override;
};

//: Given two consecutive edges, find the common incident vertex.
inline bmsh3d_vertex* incident_V_of_Es(const bmsh3d_edge* E0, const bmsh3d_edge* E1)
{
  bmsh3d_vertex* V = E0->eV();
  if (E1->is_V_incident(V))
    return V;
  V = E0->sV();
  if (E1->is_V_incident(V))
    return V;
  return nullptr;
}

//: Return the first found vertex that is incident to both E1 and E2.
inline bmsh3d_vertex* Es_sharing_V(const bmsh3d_edge* E1, const bmsh3d_edge* E2)
{
  if (E1->is_V_incident(E2->sV()))
    return E2->sV();
  if (E1->is_V_incident(E2->eV()))
    return E2->eV();
  return nullptr;
}

//: Return the first found vertex that is incident to both E1 and E2.
//  Also determine if E1 and E2 are in a loop of two edges.
inline bmsh3d_vertex* Es_sharing_V_check(const bmsh3d_edge* E1, const bmsh3d_edge* E2,
                                         bool& loop)
{
  bmsh3d_vertex* V1 = nullptr;
  bmsh3d_vertex* V2 = nullptr;
  loop = false;
  if (E1->is_V_incident(E2->sV()))
    V1 = E2->sV();
  if (E1->is_V_incident(E2->eV()))
    V2 = E2->eV();
  if (V1 && V2)
    loop = true;

  if (V1)
    return V1;
  else if (V2)
    return V2;
  else
    return nullptr;
}

inline bool same_incident_Vs(const bmsh3d_edge* E1, const bmsh3d_edge* E2)
{
  if (E1->sV() == E2->sV() && E1->eV() == E2->eV())
    return true;
  if (E1->sV() == E2->eV() && E1->eV() == E2->sV())
    return true;
  return false;
}

#endif
