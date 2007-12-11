// This is brl/bbas/bmsh3d/bmsh3d_graph.h
//---------------------------------------------------------------------
#ifndef dbmsh3d_graph_h_
#define dbmsh3d_graph_h_
//:
// \file
// \brief Graph
//
// \author
//  MingChing Chang  July 05, 2005
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

#include <dbmsh3d/dbmsh3d_vertex.h>
#include <dbmsh3d/dbmsh3d_edge.h>

#include <dbmsh3d/dbmsh3d_pt_set.h>

class dbmsh3d_graph : public dbmsh3d_pt_set
{
 protected:
  vcl_map<int, dbmsh3d_edge*> edgemap_;
  int   edge_id_counter_;

 public:
  //###### Constructor/Destructor ######
  dbmsh3d_graph () : dbmsh3d_pt_set () {
    edge_id_counter_ = 0;
  }

  virtual void clear () {
    vcl_map<int, dbmsh3d_edge*>::iterator it =  edgemap_.begin();
    for (; it != edgemap_.end(); it++) {
      _del_edge ((*it).second);
    }
    edgemap_.clear ();

    if (b_free_objects_in_destructor_) {
      vcl_map<int, dbmsh3d_vertex*>::iterator it = vertexmap_.begin();
      for (; it != vertexmap_.end(); it++)
        _del_vertex ((*it).second);
      vertexmap_.clear();
    }
    b_free_objects_in_destructor_ = false;

    dbmsh3d_pt_set::clear();
  }

  virtual ~dbmsh3d_graph () {
    clear ();
  }

  //###### Data access functions ######
  vcl_map<int, dbmsh3d_edge*>& edgemap() {
    return edgemap_;
  }
  dbmsh3d_edge* edgemap (const int i) {
    vcl_map<int, dbmsh3d_edge*>::iterator it = edgemap_.find (i);
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

  //###### Connectivity Modification Functions ######

  virtual void _del_vertex (dbmsh3d_vertex* V) {
    delete V;
  }

  //: new/delete function of the class hierarchy
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev) {
    return new dbmsh3d_edge (sv, ev, edge_id_counter_++);
  }
  virtual dbmsh3d_edge* _new_edge (dbmsh3d_vertex* sv, dbmsh3d_vertex* ev, const int id) {
    return new dbmsh3d_edge (sv, ev, id);
  }
  virtual void _del_edge (dbmsh3d_edge* E) {
    delete E;
  }

  void _add_edge (dbmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, dbmsh3d_edge*>(E->id(), E));
  }

  void add_edge_incidence (dbmsh3d_edge* E) {
    //Add edge.
    _add_edge (E);
    //Add the two edge-vertex incidences.
    dbmsh3d_vertex* sV = E->sV();
    sV->add_incident_E (E);
    dbmsh3d_vertex* eV = E->eV();
    eV->add_incident_E (E);
  }

  void add_edge_incidence_check (dbmsh3d_edge* E) {
    edgemap_.insert (vcl_pair<int, dbmsh3d_edge*>(E->id(), E));
    dbmsh3d_vertex* sV = E->sV();
    sV->check_add_incident_E (E);
    dbmsh3d_vertex* eV = E->eV();
    eV->check_add_incident_E (E);
  }

  //: Create and add the new edge to the mesh
  dbmsh3d_edge* add_new_edge (dbmsh3d_vertex* V1, dbmsh3d_vertex* V2) {
    dbmsh3d_edge* E = _new_edge (V1, V2);
    add_edge_incidence (E);
    return E;
  }

  //: vidx==0: Start Vertex, vidx==1: End Vertex,
  void _connect_edge_vertex (dbmsh3d_edge* E, const unsigned int vidx, dbmsh3d_vertex* V) {
    E->set_vertex (vidx, V);
    V->add_incident_E (E);
  }

  void _disconnect_edge_vertex (dbmsh3d_edge* E, const unsigned int vidx) {
    E->vertices(vidx)->del_incident_E (E);
    E->set_vertex (vidx, NULL);
  }
  void _disconnect_vertex_edge (dbmsh3d_vertex* V, dbmsh3d_edge* E) {
    if (V == E->sV())
      _disconnect_edge_vertex (E, 0);
    else if (V == E->eV())
      _disconnect_edge_vertex (E, 1);
    else
      assert (0);
  }

  //: Special case of graph loop
  void _connect_loop_vertex (dbmsh3d_edge* L, dbmsh3d_vertex* LV) {
    L->set_vertex (0, LV);
    L->set_vertex (1, LV);
    LV->add_incident_E (L);
  }
  bool _disconnect_loop_vertex (dbmsh3d_edge* L, dbmsh3d_vertex* LV) {
    L->set_vertex (0, NULL);
    L->set_vertex (1, NULL);
    return LV->del_incident_E (L);
  }

  //###### High-Level Connectivity Modification Functions ######

  //: delete vertex from the map and release its memory
  virtual void remove_vertex (dbmsh3d_vertex* V) {
    //The vertex can be deleted only when there's no incident edges or faces.
    assert (V->has_incident_Es() == false);
    vertexmap_.erase (V->id());
    //Delete the vertex using the virtual del function.
    _del_vertex (V);
  }
  virtual void remove_vertex (int id) {
    dbmsh3d_vertex* V = vertexmap (id);
    remove_vertex (V);
  }

  void remove_edge (dbmsh3d_edge* E) {
    //The edge can be deleted only when there's no incident faces (or halfedges).
    assert (E->n_incident_Fs() == 0);
    //Disconnect E from the two vertices
    _disconnect_edge_vertex (E, 0);
    //For loop-edge, the two vertices are the same.
    //In this case the eV here is already NULL.
    if (E->eV())
      _disconnect_edge_vertex (E, 1);
    edgemap_.erase (E->id());
    //Delete the edge using the virtual del function.
    _del_edge (E);
  }
  void remove_edge (int id) {
    dbmsh3d_edge* E = edgemap (id);
    remove_edge (E);
  }

  //: if the vertex is isolated, it will be removed, else, it is connected to some edge and will be kept.
  bool try_remove_vertex (dbmsh3d_vertex* V) {
    if (V->has_incident_Es() == false) {
      remove_vertex (V->id());
      return true;
    }
    else
      return false;
  }

  void remove_edge_complete (dbmsh3d_edge* E) {
    dbmsh3d_vertex* sV = E->sV();
    dbmsh3d_vertex* eV = E->eV();
    remove_edge (E->id());

    try_remove_vertex (sV);
    try_remove_vertex (eV);
  }

  bool try_remove_edge (dbmsh3d_edge* E) {
    if (E->halfedge() == NULL) {
      remove_edge (E->id());
      return true;
    }
    else
      return false;
  }

  //###### Other functions ######
  virtual bool brute_force_validation ();
  void summary_report ();
};

//###### Graph Processing ######

//: Remove all loops of the input graph G.
void remove_graph_loops (dbmsh3d_graph* G);

void clone_graph (dbmsh3d_graph* targetG, dbmsh3d_graph* inputG);

#endif
