// This is bmsh3d/bmsh3d_mesh.h

#ifndef _bmsh3d_mesh_h_
#define _bmsh3d_mesh_h_
//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_mesh.h
//:
// \file
// \brief mesh
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

#include <vcl_cassert.h>

#include <vcl_map.h>
#include <vcl_utility.h>

#include "bmsh3d_pt_set.h"
#include "bmsh3d_face.h"
#include "bmsh3d_face_mc.h"

// ============================================================================
// bmsh3d_ifs_mesh
// ============================================================================

//: The mesh class that handles indexed face set.
class bmsh3d_ifs_mesh : public bmsh3d_pt_set
{
protected:
  //: Dynamic mesh face data structure.
  vcl_map<int, bmsh3d_face*> facemap_;

  //: traversal position of next face
  vcl_map<int, bmsh3d_face* >::iterator face_traversal_pos_;

  int face_id_counter_;

public:
  //###### Constructor/Destructor ######
  bmsh3d_ifs_mesh () : bmsh3d_pt_set () {
    face_id_counter_ = 0;
  }

  //: copy constructor
  bmsh3d_ifs_mesh (const bmsh3d_ifs_mesh& ifs_mesh) 
    : face_id_counter_(0) {
    vcl_map<int, bmsh3d_face*> fmap = ifs_mesh.facemap_;
    vcl_map<int, bmsh3d_face*>::iterator face_it = fmap.begin();
    while (face_it != fmap.end()) {
      bmsh3d_face* face = face_it->second;
      bmsh3d_face* new_face = new bmsh3d_face(*face);
      this->_add_face(new_face);
      face_it++;
    }
  }

  bmsh3d_ifs_mesh (bool b_free_objects) :
      bmsh3d_pt_set (b_free_objects) {
    face_id_counter_ = 0;
  }

  void _clear_facemap () {
    //: Note that in some case the object is already released.
    //  In this case, we don't need to free the memory.
    face_id_counter_ = 0;
    if (b_free_objects_in_destructor_) {
      vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
      for (; it != facemap_.end(); it++)
        _del_face ((*it).second);
      facemap_.clear();
    }
  }

  virtual ~bmsh3d_ifs_mesh () {
    _clear_facemap ();
  }

  //###### Data access functions ######
  vcl_map<int, bmsh3d_face*>& facemap() {
    return facemap_;
  }
  bmsh3d_face* facemap (const int i) {
    vcl_map<int, bmsh3d_face*>::iterator it = facemap_.find (i);
    if (it == facemap_.end())
      return NULL;
    return (*it).second;
  }
  const int face_id_counter () const {
    return face_id_counter_;
  }
  void set_face_id_counter (int counter) {
    face_id_counter_ = counter;
  }
  
  bool contains_F (const int fid) {
    vcl_map<int, bmsh3d_face*>::iterator it = facemap_.find (fid);
    return it != facemap_.end();
  }

  //###### Face traversal functions ######
  void reset_face_traversal() {
    this->face_traversal_pos_ = this->facemap_.begin();
  }

  bool next_face(bmsh3d_face* &face) {
    if (this->face_traversal_pos_ == this->facemap_.end()) {
      face = NULL;
      return false;
    }
    face = (this->face_traversal_pos_->second);
    ++ (this->face_traversal_pos_);
    return true;
  }  
  
  //###### Connectivity Query Functions ######
  virtual double get_avg_edge_len_from_F ();

  //###### Connectivity Modification Functions ######
  //: new/delete function of the class hierarchy
  virtual bmsh3d_vertex* _new_vertex () {
    return new bmsh3d_vertex (vertex_id_counter_++);
  }
  virtual bmsh3d_vertex* _new_vertex (const int id) {
    if (vertex_id_counter_ <= id)
      vertex_id_counter_ = id+1;
    return new bmsh3d_vertex (id);
  }
  virtual void _del_vertex (bmsh3d_vertex* V) {
    delete V;
  }

  //: new/delete function of the class hierarchy
  virtual bmsh3d_face* _new_face () {
    return new bmsh3d_face (face_id_counter_++);
  }
  virtual bmsh3d_face* _new_face (const int id) {
    if (face_id_counter_ <= id)
      face_id_counter_ = id+1;
    return new bmsh3d_face (id);
  }

  //: new/delete function of the class hierarchy
  virtual bmsh3d_face_mc* _new_mc_face () {
    return new bmsh3d_face_mc (face_id_counter_++);
  }

  virtual void _del_face (bmsh3d_face* F) {
    delete F;
  }
  void _add_face (const bmsh3d_face* F) {
    facemap_.insert (vcl_pair<int, bmsh3d_face*>(F->id(), (bmsh3d_face*) F));
  }

  //###### Other functions ######

  //: Count the size of mesh faces indices for visualization using SoIndexedFaceSet.
  unsigned int _count_faces_indices_ifs();

  //  Count the size of mesh faces indices but skip the unmarked faces.
  unsigned int _count_visited_faces_indices_ifs(); 

  //: Assign IFS_vidx using the vertex ids
  void ifs_assign_Vs_vid_by_id ();

  //: Assign IFS_vid using the order of the vertex in the map
  // The first vertex will have vid_ = 0 and the last in the map
  // will have vid_ = (num_vertices - 1);
  void assign_IFS_vertex_vid_by_vertex_order();

  void mark_unmeshed_pts ();
  void delete_unmeshed_pts ();

  unsigned int count_unmeshed_pts () {
    vcl_map<int, bmsh3d_vertex*>::iterator it = vertexmap_.begin();
    unsigned int count=0;
    for (; it != vertexmap_.end(); it++) {
      bmsh3d_vertex* V = (*it).second;
      if (V->b_meshed() == false)
        count++;
    }
    return count;
  }

  ///virtual bool valid_conn ();
};

void move_faces_to_set (vcl_vector<vcl_vector<int> >& from_faces, 
                        vcl_vector<vcl_vector<int> >& to_faces);

// ============================================================================
// bmsh3d_mesh
// ============================================================================

class bmsh3d_mesh : public bmsh3d_ifs_mesh
{
protected:
  vcl_map<int, bmsh3d_edge* > edgemap_;
  int     edge_id_counter_;

  // traversal iterator
  vcl_map<int, bmsh3d_edge* >::iterator edge_traversal_pos_;

  //: Mesh traversing counter.
  int     i_traverse_flag_;
  
  bool    b_watertight_;

public:
  //###### Constructor/Destructor ######
  bmsh3d_mesh() {
    edge_id_counter_ = 0;
    i_traverse_flag_ = 0;
    b_watertight_ = false;
  }

  //: copy constructor
  bmsh3d_mesh(const bmsh3d_mesh& mesh);

  bmsh3d_mesh (bool b_free_objects) :
      bmsh3d_ifs_mesh (b_free_objects) {
    edge_id_counter_ = 0;
    i_traverse_flag_ = 0;
  }

  virtual bmsh3d_mesh* clone() const { return new bmsh3d_mesh(*this); }

  //: function to disconnect & remove all faces from the facemap_
  void clear_faces () {
    face_id_counter_ = 0;
    vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
    while (it != facemap_.end()) {
      bmsh3d_face* F = (*it).second;
      remove_face (F);      
      it = facemap_.begin();
    }
  }

  void clear_edges () {
    edge_id_counter_ = 0; 
    vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.begin();
    while (it != edgemap_.end()) {
      bmsh3d_edge* E = (*it).second;
      remove_edge (E);
      it = edgemap_.begin();
    }
  }

  virtual ~bmsh3d_mesh () {
    //Note that in some case the object is already released.
    //In this case, we don't need to free the memory.
    if (b_free_objects_in_destructor_) {
      //delete all faces first.
      clear_faces ();
      //delete all edges
      clear_edges();         
      //delete all vertices
      vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
      for (; vit != vertexmap_.end(); vit++)
        _del_vertex ((*vit).second);
      vertexmap_.clear();
    }
    b_free_objects_in_destructor_ = false;
  }

  //###### Data Access Functions ######
  vcl_map<int, bmsh3d_edge*>& edgemap() {
    return edgemap_;
  }
  bmsh3d_edge* edgemap (const int i) {
    vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.find (i);
    if (it == edgemap_.end())
      return NULL;
    return (*it).second;
  }

  const int edge_id_counter() const {
    return edge_id_counter_;
  }
  void set_edge_id_counter (int counter) {
    edge_id_counter_ = counter;
  }
  bool b_watertight () const {
    return b_watertight_;
  }
  
  bool contain_e (const int eid) {
    vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.find (eid);
    return it != edgemap_.end();
  }

  //: Return true if the mesh representation is MHE.
  //  Otherwise return false (for IFS).
  bool is_MHE() const {
    return edgemap_.size() != 0;
  }

  //###### Edge Traversal ######
  void reset_edge_traversal()
  {
    this->edge_traversal_pos_ = this->edgemap_.begin();
  }

  bool next_edge(bmsh3d_edge* &E)
  {
    if (this->edge_traversal_pos_ == this->edgemap_.end()) 
    {
      E = 0;
      return false;
    }
    E = (this->edge_traversal_pos_->second);
    ++ (this->edge_traversal_pos_);
    return true;
  }  

  //###### Connectivity Query Functions ######
  virtual double get_avg_edge_len_from_F ();

  //: Count the size of mesh faces indices for visualization using SoIndexedFaceSet.
  unsigned int count_faces_indices();
  unsigned int _count_faces_indices_mhe();

  //###### Connectivity Modification Functions ######

  //: new/delete function of the class hierarchy
  virtual bmsh3d_edge* _new_edge (bmsh3d_vertex* sV, bmsh3d_vertex* eV) {
    return new bmsh3d_edge (sV, eV, edge_id_counter_++);
  }
  virtual void _del_edge (bmsh3d_edge* E) {
    delete E;
  }
  void _add_edge (bmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, bmsh3d_edge*>(E->id(), E));
  }

  void add_edge_incidence (bmsh3d_edge* E) {
    //Add edge.
    _add_edge (E);
    //Add the two edge-vertex incidences.
    bmsh3d_vertex* sV = E->sV();
    sV->add_incident_E (E);
    bmsh3d_vertex* eV = E->eV();
    eV->add_incident_E (E);
  }

  void add_edge_incidence_check (bmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, bmsh3d_edge*>(E->id(), E));
    bmsh3d_vertex* sV = E->sV();
    sV->check_add_incident_E (E);
    bmsh3d_vertex* eV = E->eV();
    eV->check_add_incident_E (E);
  }
  
  //: Create and add the new edge to the mesh
  bmsh3d_edge* add_new_edge (bmsh3d_vertex* V1, bmsh3d_vertex* V2) {    
    bmsh3d_edge* E = _new_edge (V1, V2);
    add_edge_incidence (E);
    return E;
  }

  void _disconnect_edge_vertex (bmsh3d_edge* E, const unsigned int vidx) {
    E->vertices(vidx)->del_incident_E (E);
    E->set_vertex (vidx, NULL);    
  }
  void _disconnect_vertex_edge (bmsh3d_vertex* V, bmsh3d_edge* E) {
    if (V == E->sV())
      _disconnect_edge_vertex (E, 0);
    else if (V == E->eV())
      _disconnect_edge_vertex (E, 1);
    else
      assert (0);
  }

  void _update_incident_Es_Vs ();

  //###### High-Level Connectivity Modification Functions ######

  //: add a new vertex to the map
  bmsh3d_vertex* add_vertex(const vgl_point_3d<double >& pt)
  {
    bmsh3d_vertex* V = (bmsh3d_vertex*)(this->_new_vertex());
    V->set_pt(pt);
    this->_add_vertex (V);
    return V;
  }

  virtual void remove_vertex (bmsh3d_vertex* V) {
    //Delete a vertex only when there's no incident edges or faces.
    assert (V->has_incident_Es() == false);
    vertexmap_.erase (V->id());
    //Delete V using the virtual del function.
    _del_vertex (V);
  }
  //: delete vertex from the map and release its memory
  virtual void remove_vertex (int id) {
    bmsh3d_vertex* V = vertexmap (id);
    remove_vertex (V);
  }

  void remove_edge (bmsh3d_edge* E) {
    //An edge can be deleted only when there's no incident faces (or halfedges).
    assert (E->n_incident_Fs() == 0);
    //disconnect from the two vertices
    _disconnect_edge_vertex (E, 0);
    _disconnect_edge_vertex (E, 1);
    edgemap_.erase (E->id());    
    _del_edge (E); //delete the face using the virtual del function.
  }
  void remove_edge (int id) {
    bmsh3d_edge* E = edgemap (id);
    remove_edge (E);
  }

  //: remove a mesh face: disconnect all halfedges and delete them.
  void remove_face (bmsh3d_face* F) {    
    F->_discon_all_incident_Es ();
    facemap_.erase (F->id());
    _del_face (F);
  }

  //: remove a mesh face: disconnect all halfedges and delete them.
  void remove_face (int id) {
    bmsh3d_face* F = facemap (id);
    remove_face (F);
  }

  void remove_F_del_isolated_Es (bmsh3d_face* F);

  bmsh3d_face* add_new_face (const vcl_vector<bmsh3d_edge*>& ordered_edges);

  //###### Connectivity Recovery Functions ######
  //: Build the Modified Half-edge datastructure from the IndexedFaceSet representation.
  void IFS_to_MHE ();
  void IFS_to_MHE_build_edges (const bool skip_v0 = false);
    void sort_halfedges_for_all_faces ();

  void IFS_to_MHE_bf (const bool skip_v0 = false);

  //: Assume the the HE data structure for this mesh is complete
  //  This function build the IFS from the HE data structure.
  void build_face_IFS (); 
  void build_IFS_mesh ();
  void clean_IFS_mesh ();

  void MHE_to_IFS ();

  void sort_V_incident_Es ();

  ///virtual bool valid_conn ();
  
  //###### Mesh Traverse Functions ######
  const int i_traverse_flag() const {
    return i_traverse_flag_;
  }
  //: increase the search flag, so that every element has lower value than the flag (unvisited)
  //  once visited, it's i_visited_ will be set to i_traverse_flag_ (visited)
  int init_traverse() {
    return ++i_traverse_flag_;
  }

  void reset_traverse_v () {
    vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
    for (; vit != vertexmap_.end(); vit++) {
      bmsh3d_vertex* V = (*vit).second;
      V->set_i_visited (0);
    }
    i_traverse_flag_ = 1;
  }
  void reset_traverse_e () {
    vcl_map<int, bmsh3d_edge*>::iterator eit = edgemap_.begin();
    for (; eit != edgemap_.end(); eit++) {
      bmsh3d_edge* E = (*eit).second;
      E->set_i_visited (0);
    }
    i_traverse_flag_ = 1;
  }
  void reset_traverse_f () {
    vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
    for (; fit != facemap_.end(); fit++) {
      bmsh3d_face* F = (*fit).second;
      F->set_i_visited (0);
    }
    i_traverse_flag_ = 1;
  }
  void reset_traverse () {
    reset_traverse_v ();
    reset_traverse_e ();
    reset_traverse_f ();
  }

  //###### Other functions ######
    
  // Print a summary of mesh properties
  virtual void print_summary (vcl_ostream& str);
  void print_topo_summary (void);
  
  //: return if the mesh is a 2-manifold
  bool is_2_manifold ();

  //: return if the mesh is a triangular mesh
  unsigned int count_max_polygon_sides ();

  //: Return number of edges in traversing the IFS data structure.
  //  Note that an internal edge can be used twice.
  unsigned int count_ifs_dup_edges ();

  //: return number of edges at the mesh boundary.
  unsigned int count_bnd_edges (bool& b_2_manifold);

  double get_avg_edge_len ();

  //: Merge two faces who sharing an edge together.
  //  Only valid if E is a 2-manifold edge.
  void m2_mesh_merge_face (bmsh3d_face* F1, bmsh3d_face* F2 ,bmsh3d_edge* E);
};

bmsh3d_halfedge* _connect_F_E_end (bmsh3d_face* face, bmsh3d_edge* E); 

bmsh3d_mesh* clone_mesh_ifs_3d (bmsh3d_mesh* M);

bmsh3d_face* add_F_to_M (vcl_vector<int>& vids, bmsh3d_mesh* M);

bmsh3d_face* add_F_to_M_check_topo (vcl_vector<int>& vids, bmsh3d_mesh* M);

void add_M_faces_to_IFSset (bmsh3d_mesh* M, vcl_vector<vcl_vector<int> >& faces);

void bmsh3d_mesh_print_object_size ();

//: for 2-manifold mesh only
void manifold_fix_faces_orientation (bmsh3d_mesh* mesh, int sfaceid, bool b_flip_sface);


#endif

