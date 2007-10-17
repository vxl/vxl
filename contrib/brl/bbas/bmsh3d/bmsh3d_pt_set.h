#ifndef bmsh3d_pt_set_h_
#define bmsh3d_pt_set_h_
//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_pt_set.h
//:
// \file
// \brief 3d point set
//
//
// \author
//  MingChing Chang  Feb 10, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------
#include <vcl_cassert.h>

#include <vcl_map.h>
#include <vcl_utility.h>

#include "bmsh3d_vertex.h"

class bmsh3d_pt_set
{
protected:
  //: The modified-halfedg mesh vertex data structure.
  vcl_map<int, bmsh3d_vertex*> vertexmap_;

  //: traversal position of next vertex
  vcl_map<int, bmsh3d_vertex* >::iterator vertex_traversal_pos_;

  int vertex_id_counter_;

  bool b_free_objects_in_destructor_;

public:
  //###### Constructor/Destructor ######
  bmsh3d_pt_set () {
    b_free_objects_in_destructor_ = true;
    vertex_id_counter_ = 0;
  }
  bmsh3d_pt_set (bool b_free_objects) {
    b_free_objects_in_destructor_ = b_free_objects;
    vertex_id_counter_ = 0;
  }

  //: if you get free memory error, check if use 
  //  pointset->_new_vertex() instead of using 'new bmsh3d_vertex' in the code
  //  for each object
  virtual void clear () {    
    vertex_id_counter_ = 0;
    if (b_free_objects_in_destructor_) { //Skip already released objects.    
      vcl_map<int, bmsh3d_vertex*>::iterator it = vertexmap_.begin();
      for (; it != vertexmap_.end(); it++)
        _del_vertex ((*it).second);
      vertexmap_.clear();
    }
  }

  virtual ~bmsh3d_pt_set() {
    clear ();
  }

  //###### Data access functions ######

  unsigned int num_vertices() const { 
    return this->vertexmap_.size(); 
  }

  vcl_map<int, bmsh3d_vertex*>& vertexmap() {
    return vertexmap_;
  }
  bmsh3d_vertex* vertexmap (const int i) {
    vcl_map<int, bmsh3d_vertex*>::iterator it = vertexmap_.find (i);
    if (it == vertexmap_.end())
      return NULL;
    return (*it).second;
  }

  const int vertex_id_counter() const {
    return vertex_id_counter_;
  }
  void set_vertex_id_counter (const int counter) {
    vertex_id_counter_ = counter;
  }
  const bool b_free_objects_in_destructor() const {
    return b_free_objects_in_destructor_;
  }
  void set_free_objects_in_destructor (const bool b) {
    b_free_objects_in_destructor_ = b;
  }  

  bool contains_V (const int vid) {
    vcl_map<int, bmsh3d_vertex*>::iterator it = vertexmap_.find (vid);
    return it != vertexmap_.end();
  }

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
  virtual void _del_vertex (bmsh3d_vertex* v) {
    delete v;
  }

  void _add_vertex (bmsh3d_vertex* V) {
    vertexmap_.insert (vcl_pair<int, bmsh3d_vertex*>(V->id(), V));
  }

  void reset_vertices_ids ();

  //###### Vertex traversal functions ######
  //: initialize vertex traversal
  void reset_vertex_traversal() { 
    this->vertex_traversal_pos_ = this->vertexmap_.begin(); 
  }

  //: get the next vertex. Return false if no more vertex left on the list
  bool next_vertex(bmsh3d_vertex* &v) { 
    if (this->vertex_traversal_pos_ == this->vertexmap_.end()) return false;
    v = this->vertex_traversal_pos_->second;
    ++ this->vertex_traversal_pos_;
    return true;
  }

  //: Reset all vertices' i_value_ to 0.
  void reset_vertices_i_value (const int i_value);

  //###### High-Level Connectivity Modification Functions ######

  //: delete vertex from the map and release its memory
  virtual void remove_vertex (bmsh3d_vertex* V) {
    remove_vertex (V->id());
  }
  virtual void remove_vertex (int id) {
    bmsh3d_vertex* vertex = vertexmap (id);
    vertexmap_.erase (id);
    _del_vertex (vertex);
  }
};

void clone_ptset (bmsh3d_pt_set* targetPS, bmsh3d_pt_set* inputPS);

bool detect_bounding_box (bmsh3d_pt_set* pt_set, vgl_box_3d<double>& bounding_box); 

bool detect_geom_center (bmsh3d_pt_set* pt_set, vgl_point_3d<double>& C);

bmsh3d_pt_set* clone_pt_set_3d (bmsh3d_pt_set* PS);

void remove_duplicate_points (bmsh3d_pt_set* pt_set);

//: Other processing functions
void translate_points (bmsh3d_pt_set* pt_set, 
                       const float tx, const float ty, const float tz);

void rotate_points (bmsh3d_pt_set* pt_set, 
                    const float rx, const float ry, const float rz);

void scale_points (bmsh3d_pt_set* pt_set, const float scale);

void perturb_points (bmsh3d_pt_set* pt_set, const float pert);

void crop_points (bmsh3d_pt_set* pt_set, 
                  const float minX, const float minY, const float minZ, 
                  const float maxX, const float maxY, const float maxZ);

void shift_points_to_first_octant (bmsh3d_pt_set* pt_set);

void subsample_points (bmsh3d_pt_set* pt_set, const unsigned int subsam_pts);

#endif

