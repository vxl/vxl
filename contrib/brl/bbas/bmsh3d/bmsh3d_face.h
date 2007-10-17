#ifndef bmsh3d_face_h_
#define bmsh3d_face_h_
//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_face.h
//:
// \file
// \brief Basic 3d face on a mesh
//
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

#include <vcl_vector.h>
#include <vgl/vgl_point_2d.h>
#include <vgl/vgl_distance.h>
#include <vgl/vgl_box_3d.h>
#include <vgl/vgl_plane_3d.h>
#include <vgl/vgl_homg_plane_3d.h>
#include <vgl/algo/vgl_fit_plane_3d.h>

#include "bmsh3d_utils.h"
#include "bmsh3d_vertex.h"
#include "bmsh3d_edge.h"

//Color code to visualize several mesh objects.
typedef enum {
  BOGUS_TRIFACE       = 0,  
  TRIFACE_111         = 1,  
  TRIFACE_112         = 2,  
  TRIFACE_113P        = 3,  
  TRIFACE_122         = 4,  
  TRIFACE_123P        = 5,  
  TRIFACE_13P3P       = 6,  
  TRIFACE_222         = 7,  
  TRIFACE_223P        = 8,  
  TRIFACE_23P3P       = 9,  
  TRIFACE_3P3P3P      = 10,
  TRIFACE_E4P         = 11,
} TRIFACE_TYPE;

class bmsh3d_mesh;

class bmsh3d_face : public vispt_elm
{
protected:
  //: Pointer to the IFS vertices of this face.
  vcl_vector<bmsh3d_vertex*> vertices_;

  bmsh3d_halfedge* halfedge_;

  int   id_;

  //: This variable is used for
  //    - i_visited_ flag for mesh traversal.
  //    - a flag in gdt propagation.
  //    - storing the sheet id.
  int  i_value_;

public:
  //###### Constructor/Destructor ######
  bmsh3d_face () {
    halfedge_ = NULL;
    i_value_  = 0;
    id_       = -1;
  }
  bmsh3d_face (bmsh3d_halfedge* he) {
    halfedge_ = he;
    i_value_  = 0;
    id_       = -1;
  }
  bmsh3d_face (const int id) {
    halfedge_ = NULL;
    id_         = id;
    i_value_  = 0;
  }

  virtual ~bmsh3d_face () {
    vertices_.clear();
    //: make sure that all halfedges are deleted before the destructor.
    //  You should use bmsh3d_mesh::delete_face to delete a face.
    assert (halfedge_ == NULL);
  }

  //###### Data access functions ######
  const bmsh3d_vertex* vertices (const unsigned int i) const {
    return vertices_[i];
  }
  bmsh3d_vertex* vertices (const unsigned int i) {
    return vertices_[i];
  }
  const vcl_vector<bmsh3d_vertex*>& vertices() const {
    return vertices_;
  }
  vcl_vector<bmsh3d_vertex*>& vertices() {
    return vertices_;
  }

  bmsh3d_halfedge* halfedge() const {
    return halfedge_;
  }
  bmsh3d_halfedge* & halfedge() {
    return halfedge_;
  }
  void set_halfedge (bmsh3d_halfedge* he) {
    halfedge_ = he;
  }
  const int id() const {
    return id_;
  }
  void set_id (const int id) {
    id_ = id;
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

  //: just use the i_value_ as a boolean flag
  bool b_visited () const {
    return (i_value_ != 0);
  }
  void set_visited (bool b) {
    if (b == false)
      i_value_ = 0;
    else
      i_value_ = 1;
  }

  const int sid () const {
    return i_value_;
  }
  void set_sid (const int sid) {
    i_value_ = sid;
  }

  //###### Connectivity Query via Halfedges ######  
  void get_incident_HEs (vcl_vector<bmsh3d_halfedge*>& incident_HEs) const;
  void get_incident_Es (vcl_vector<bmsh3d_edge*>& incident_Es) const;
  const unsigned int n_incident_Es () const;
  bool is_E_incident (const bmsh3d_edge* inputE) const;
  bmsh3d_halfedge* find_bnd_HE () const;

  bool is_V_incident_via_HE (const bmsh3d_vertex* inputV) const;
  bmsh3d_vertex* get_next_V_via_HE (const bmsh3d_vertex* inputV) const;

  //: Given a vertex V and an edge of this face incident to V,
  //  find the other edge of this face incident to V.
  bmsh3d_edge* find_other_E (const bmsh3d_vertex* inputV, 
                              const bmsh3d_edge* inputE) const;

  //: Given a vertex V and a halfedge of this face incident to V,
  //  find the other halfedge of this face incident to V.
  bmsh3d_halfedge* find_other_HE (const bmsh3d_vertex* inputV,
                                   const bmsh3d_halfedge* inputHE) const;

  //: Given a vertex V and an edge of this face incident to V,
  //  find the next edge (following the circular halfedge list) of this face incident to V.
  bmsh3d_edge* find_next_E (const bmsh3d_vertex* inputV, 
                              const bmsh3d_edge* inputE) const;

  //: Given a vertex V and a halfedge of this face incident to V,
  //  find the next halfedge (following the circular halfedge list) of this face incident to V.
  bmsh3d_halfedge* find_next_HE (const bmsh3d_vertex* inputV,
                                  const bmsh3d_halfedge* inputHE) const;

  double angle_at_V (const bmsh3d_vertex* inputV) const;

  int n_incident_Vs_in_set (vcl_set<bmsh3d_vertex*>& vertices) const;
  bool all_Vs_incident (vcl_vector<bmsh3d_vertex*>& vertices) const;

  void get_ordered_Vs (vcl_vector<bmsh3d_vertex*>& vertices) const;
    void _get_ordered_Vs_MHE (vcl_vector<bmsh3d_vertex*>& vertices) const;
    void _get_ordered_Vs_IFS (vcl_vector<bmsh3d_vertex*>& vertices) const;
  
  void get_ordered_V_ids (vcl_vector<int>& vids) const;
    void _get_ordered_V_ids_MHE (vcl_vector<int>& vids) const;
    void _get_ordered_V_ids_IFS (vcl_vector<int>& vids) const;

  //###### Handle local list of incident vertices ######
  void _ifs_add_vertex (bmsh3d_vertex* V) {
    vertices_.push_back (V);
  }
  void _ifs_clear_vertices () {
    vertices_.clear();
  }
  bmsh3d_vertex* _ifs_prev_V (bmsh3d_vertex* inputV) {
    if (vertices_[0] == inputV)
      return vertices_[vertices_.size()-1];

    for (unsigned int i=1; i<vertices_.size(); i++) {
      if (vertices_[i] == inputV)
        return vertices_[i-1];
    }
    assert (0);
    return NULL;
  }
  bmsh3d_vertex* _ifs_next_V (bmsh3d_vertex* inputV) {
    if (vertices_[vertices_.size()-1] == inputV)
      return vertices_[0];

    for (unsigned int i=0; i<vertices_.size()-1; i++) {
      if (vertices_[i] == inputV)
        return vertices_[i+1];
    }
    assert (0);
    return NULL;
  }
  //: track incident vertices and reset ifs_face::vertices_[]
  void _ifs_track_ordered_vertices ();  

  void _ifs_assign_Vs_vid_by_id () {
    for (unsigned int i=0; i<vertices_.size(); i++) {
      bmsh3d_vertex* V = vertices_[i];
      V->set_vid (V->id());
    }
  }

  //Test if the face's IFS structure is correct (repeated or wrong Vids).
  bool _is_ifs_valid (bmsh3d_mesh* M);

  bool _ifs_inside_box (const vgl_box_3d<double>& box) const;
  bool _ifs_outside_box (const vgl_box_3d<double>& box) const;

  //###### Geometry Query Functions ######
  bool is_inside_box (const vgl_box_3d<double>& box) const;
  bool is_outside_box (const vgl_box_3d<double>& box) const;
  vgl_point_3d<double> compute_center_pt () const;
  vgl_point_3d<double> compute_center_pt (const vcl_vector<bmsh3d_vertex*>& vertices) const;
  vgl_vector_3d<double> compute_normal ();

  //###### Connectivity Modification Functions ######
  
  //: Connect a halfedge to this mesh face.
  void _connect_HE_to_end (bmsh3d_halfedge* inputHE);
  //: Remove the input halfedge from the face's halfedge list.
  bool _remove_HE (bmsh3d_halfedge* inputHE);
  //: Create a halfedge and connect a mesh face and an edge.
  void connect_E_to_end (bmsh3d_edge* E);  
  //: Disconnect a face and an edge (delete the halfedge).
  void disconnect_E (bmsh3d_halfedge* HE);

  //: Sort the incident halfedges to form a circular list
  bool _sort_HEs_circular ();

  //: disconnect all associated halfedges from their edges and delete them.
  virtual void _discon_all_incident_Es ();
  
  //: reverse the orientation of chain of halfedges of this face.
  void _reverse_HE_chain ();

  void set_orientation (bmsh3d_halfedge* new_start_he,
                        bmsh3d_vertex*   new_next_v);
  
  //###### Other functions ######
  virtual void getInfo (vcl_ostringstream& ostrm);

  //###### For triangular face only ######
  TRIFACE_TYPE tri_get_topo_type () const;
  vcl_string   tri_get_topo_string() const;

  //###### For the face of a 2-manifold triangular mesh only ######
  //  these functions start with tag m2t (manifold-2-triangle)

  bmsh3d_edge* m2t_edge_against_vertex (bmsh3d_vertex* input_vertex);
  bmsh3d_halfedge* m2t_halfedge_against_vertex (bmsh3d_vertex* input_vertex);

  //: given input_face, find the neighboring face against the input_vertex
  bmsh3d_face* m2t_nbr_face_against_vertex (bmsh3d_vertex* input_vertex);

  bmsh3d_face* m2t_nbr_face_sharing_edge (bmsh3d_vertex* v1, bmsh3d_vertex* v2);

  //: given v1, v2, find v3
  bmsh3d_vertex* t_3rd_vertex (const bmsh3d_vertex* V1, const bmsh3d_vertex* V2) const;
  bmsh3d_vertex* t_vertex_against_edge (const bmsh3d_edge* inputE) const;

};

//###### Additional Functions ######

bmsh3d_halfedge* _find_prev_in_HE_chain (const bmsh3d_halfedge* inputHE);

//: disconnect all associated halfedges from their edges from the given he_head.
void _delete_HE_chain (bmsh3d_halfedge* & he_head);

//  Return: the set of incident edges that get disconnected.
//  Also set the he_head to be NULL after calling it.
void _delete_HE_chain (bmsh3d_halfedge* & he_head,
                       vcl_vector<bmsh3d_edge*>& incident_edge_list);

//: Given the face, current halfedge, and current eV,
//  find the next halfedge given in the vector<>.
bmsh3d_halfedge* _find_next_halfedge (bmsh3d_halfedge* HE, 
                                       bmsh3d_vertex* eV, 
                                       vcl_vector<bmsh3d_halfedge*>& inc_hes);

//: Assume the mesh face is planar and compute a 2D planar coordinate for it.
void get_2d_coord (const vcl_vector<bmsh3d_vertex*>& vertices,
                   vgl_vector_3d<double>& N, vgl_vector_3d<double>& AX,
                   vgl_vector_3d<double>& AY);

//: Return ordered set of vertices in 2D (x,y) coord.
void get_2d_polygon (const vcl_vector<bmsh3d_vertex*>& vertices,
                     vcl_vector<double>& xs, vcl_vector<double>& ys);

//: Return the projected point in the local 2D (x,y) coord.
vgl_point_2d<double> get_2d_proj_pt (vgl_point_3d<double> P, const vgl_point_3d<double>& A,
                                     const vgl_vector_3d<double>& AX, 
                                     const vgl_vector_3d<double>& AY);

vgl_point_3d<double> compute_cen (const vcl_vector<bmsh3d_vertex*>& vertices);

vgl_vector_3d<double> compute_normal_ifs (const vcl_vector<bmsh3d_vertex*>& vertices);

//: Compute face normal using the given edge and starting node.
vgl_vector_3d<double> compute_normal (const vgl_point_3d<double>& C,
                                      const bmsh3d_edge* E, 
                                      const bmsh3d_vertex* Es);

//: Return true if vertices is a polygon or obtuse triangle.
bool is_tri_non_acute (const vcl_vector<bmsh3d_vertex*>& vertices);

bool is_F_extraneous (bmsh3d_face* F);

bmsh3d_face* get_F_sharing_Es (bmsh3d_edge* E1, bmsh3d_edge* E2);

//###### For the face of a 2-manifold triangular mesh only ######

inline void m2t_compute_tri_angles (const double& c, const double& l, const double& r,
                                    double& angle_cl, double& angle_cr, double& angle_lr)
{
  angle_cl = vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
  angle_cr = vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
  angle_lr = vcl_acos ( (l*l + r*r - c*c)/(l*r*2) );
}

inline void m2t_compute_angles_cl_cr (const double& c, const double& l, const double& r,
                                      double& angle_cl, double& angle_cr)
{
  angle_cl = vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
  angle_cr = vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
}

inline double m2t_compute_angle_cl (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (c*c + l*l - r*r)/(c*l*2) );
}

inline double m2t_compute_angle_cr (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (c*c + r*r - l*l)/(c*r*2) );
}

inline double m2t_compute_angle_lr (const double& c, const double& l, const double& r)
{
  return vcl_acos ( (l*l + r*r - c*c)/(l*r*2) );
}


#endif

