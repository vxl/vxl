// This is brl/bbas/bmsh3d/bmsh3d_vertex.h
//---------------------------------------------------------------------
#ifndef _bmsh3d_vertex_h_
#define _bmsh3d_vertex_h_
//:
// \file
// \brief Basic 3d point sample on a mesh
//
// \author
//  MingChing Chang  April 22, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_set.h>
#include <vcl_string.h>
#include <vcl_cassert.h>
#include <vgl/vgl_point_3d.h>

#include "bmsh3d_ptr_list.h"
#include "bmsh3d_utils.h"

//#######################################################
//     The Mesh Library Data Structure
//     (Combined IndexedFaceSet and Half-Edge.)
//     Can handle:
//       1) Point Cloud
//       2) Point Cloud with Edges (Indexed Line Set)
//       3) Point Cloud with Edges and Faces (ILS and IFS)
//       4) Non-Manifold Mesh (IFS)
//       5) Manifold Mesh
//       6) Triangle Manifold Mesh
//     Need to revise the HalfEdge design by
//     changing each vertex's halfedge pointer to an edge pointer!
//#######################################################

class bmsh3d_edge;
class bmsh3d_halfedge;
class bmsh3d_face;

//: Typology of a mesh vertex.
typedef enum
{
  BOGUS_VTOPO_TYPE          = 0,
  VTOPO_ISOLATED            = 1,
  VTOPO_EDGE_ONLY           = 2,
  VTOPO_EDGE_JUNCTION       = 3,
  VTOPO_2_MANIFOLD          = 4,
  VTOPO_2_MANIFOLD_1RING    = 5,
  VTOPO_NON_MANIFOLD        = 6,
  VTOPO_NON_MANIFOLD_1RING  = 7,
} VTOPO_TYPE;

class bmsh3d_vertex : public vispt_elm
{
 protected:
  int   id_;

  vgl_point_3d<double> pt_;

  //: link list to store incident mesh edges
  bmsh3d_ptr_node* E_list_;

  //: link list to incident mesh faces for intermediate processing.
  bmsh3d_ptr_node* F_list_;

  //: To optimize C++ class object size, this variable is used for:
  //    - i_visited_: the visited flag for mesh hypergraph traversal.
  //    - vid_: for (IFS) keeping the order of vertices of a face.
  //    - b_valid_: valid or not
  int   i_value_;

  //: This variable is used for:
  //    - b_meshed_: is this vertex meshed or not.
  //    - type info for dbsk3d_fs_node_elm.
  //  It's here to optimize C++ class object size.
  char  c_value_;

  //: This variable is used in shocks.
  char flow_type_;

 public:
  //###### Constructor/Destructor ######
  bmsh3d_vertex (int id) {
    E_list_ = NULL;
    F_list_ = NULL;
    id_ = id;
    i_value_  = 0;
    c_value_  = '?';
    flow_type_ = '?';
  }
  bmsh3d_vertex (const double& x, const double& y, const double& z, const int id) {
    E_list_ = NULL;
    F_list_ = NULL;
    id_ = id;
    i_value_  = 0;
    c_value_  = '?';
    flow_type_ = '?';
    pt_.set (x, y, z);
  }
  virtual ~bmsh3d_vertex() {
    //can not delete a vertex with any incident edge
    assert (E_list_ == NULL);
  }

  //###### Data access functions ######
  const vgl_point_3d<double>& pt() const {
    return  pt_;
  }
  vgl_point_3d<double>& get_pt () {
    return  pt_;
  }
  void set_pt (const vgl_point_3d<double>& pt) {
    pt_ = pt;
  }
  void set_pt (const double& x, const double& y, const double& z) {
    pt_.set (x, y, z);
  }

  bmsh3d_ptr_node* E_list() const {
    return E_list_;
  }
  bmsh3d_ptr_node* F_list() const {
    return F_list_;
  }
  void set_F_list (bmsh3d_ptr_node* F_list) {
    F_list_ = F_list;
  }
  unsigned int get_Fs (vcl_set<void*>& ptrs) {
    return get_all_ptrs (F_list_, ptrs);
  }
  unsigned int clear_F_list () {
    return clear_ptr_list (F_list_);
  }
  void add_F (void* F) {
    bmsh3d_ptr_node* cur = new bmsh3d_ptr_node (F);
    _add_to_ptr_list_head (F_list_, cur);
  }

  int id () const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
  }

  int i_value () const {
    return i_value_;
  }
  void set_i_value (const int v) {
    i_value_ = v;
  }
  int vid() const {
    return i_value_;
  }
  void set_vid (int vid) {
    i_value_ = vid;
  }

  const bool is_visited () const {
    return i_value_ != 0;
  }
  //: if i_value_ less than i_traverse_flag, it's not visited
  const bool is_visited (const int traverse_value) const {
    if (i_value_ < traverse_value)
      return false;
    else
      return true;
  }
  void set_i_visited (const int traverse_value) {
    i_value_ = traverse_value;
  }

  const bool is_valid() const {
    return i_value_ != 0;
  }
  void set_valid (const bool v) {
    if (v)
      i_value_ = 1;
    else
      i_value_ = 0;
  }

  bool b_meshed() const {
    return c_value_ == 'm';
  }
  void set_meshed (const bool b) {
    if (b)
      c_value_ = 'm';
    else
      c_value_ = '?';
  }

  //: Return a platform-independent name of the class
  virtual vcl_string is_a() const
  {return "bmsh3d_vertex"; }

  //###### Handle the incident edges ######
  unsigned int get_incident_Es (vcl_set<void*>& incident_Es) const {
    return get_all_ptrs (E_list_, incident_Es);
  }
  unsigned int n_incident_Es() const {
    return count_all_ptrs (E_list_);
  }
  unsigned int clear_incident_E_list () {
    return clear_ptr_list (E_list_);
  }
  bool has_incident_Es() const {
    return E_list_!=NULL;
  }
  bool is_E_incident (const bmsh3d_edge* E) const {
    return is_in_ptr_list (E_list_, E);
  }
  void add_incident_E (const bmsh3d_edge* E) {
    add_ptr_to_list (E_list_, E); //add_ptr_check
  }
  bool check_add_incident_E (const bmsh3d_edge* E) {
    return check_add_ptr (E_list_, E);
  }
  bool del_incident_E (const bmsh3d_edge* E) {
    return del_ptr (E_list_, E);
  }

  bmsh3d_edge* get_1st_incident_E() const {
    if (E_list_ == NULL)
      return NULL;
    return (bmsh3d_edge*) E_list_->ptr();
  }

  bmsh3d_halfedge* get_1st_bnd_HE () const;

  //: function to return all incident faces of this vertex
  int get_incident_Fs (vcl_set<bmsh3d_face*>& face_set);

  //: return the vertex topology type
  VTOPO_TYPE detect_vtopo_type () const;
  unsigned int _check_2_manifold (const bmsh3d_edge* startE, VTOPO_TYPE& cond) const;
  bmsh3d_edge* _find_unvisited_E () const;

  //###### Other functions ######
  virtual void getInfo (vcl_ostringstream& ostrm);

  //###### For the face of a 2-manifold mesh only ######
  //  these functions start with a tag m2 (manifold-2)
  bmsh3d_halfedge* m2_get_ordered_HEs (vcl_vector<bmsh3d_halfedge*>& ordered_halfedges) const;

  bmsh3d_halfedge* m2_get_next_bnd_HE (const bmsh3d_halfedge* inputHE) const;

  //: return true if it is on the boundary of the mesh.
  //  Start tracing from input_he to see if the loop back to input_he
  bool m2_is_on_bnd (bmsh3d_halfedge* inputHE) const;

  //: return the sum_theta at this vertex
  double m2_sum_theta () const;
};

//: Find the mesh edge sharing the two vertices.
bmsh3d_edge* E_sharing_2V (const bmsh3d_vertex* V1, const bmsh3d_vertex* V2);

//: Find the mesh face sharing the given vertices.
bmsh3d_face* find_F_sharing_Vs (vcl_vector<bmsh3d_vertex*>& vertices);

bmsh3d_face* get_non_manifold_1ring_extra_Fs (bmsh3d_vertex* V);

bool is_F_V_incidence (bmsh3d_vertex* V, const bmsh3d_vertex* V1, const bmsh3d_vertex* V2);

bmsh3d_edge* V_find_other_E (const bmsh3d_vertex* V, const bmsh3d_edge* inputE);

#endif // _bmsh3d_vertex_h_
