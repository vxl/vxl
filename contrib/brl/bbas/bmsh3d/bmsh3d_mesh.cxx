//---------------------------------------------------------------------
// This is brl/bbas/bmsh3d/bmsh3d_mesh.cxx
//:
// \file
// \brief Mesh
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

#include <vcl_cstdio.h>
#include <vcl_list.h>
#include <vcl_set.h>
#include <vcl_map.h>
#include <vcl_queue.h>
#include <vcl_iostream.h>

#include <vcl_iostream.h>
#include <vcl_sstream.h>
#include <vnl/vnl_math.h>
#include <vul/vul_printf.h>

#include "bmsh3d_he_mesh.h"
#include "bmsh3d_mesh.h"

void move_faces_to_set (vcl_vector<vcl_vector<int> >& from_faces, 
                        vcl_vector<vcl_vector<int> >& to_faces)
{
  to_faces.insert (to_faces.end(), from_faces.begin(), from_faces.end());
}


double bmsh3d_ifs_mesh::get_avg_edge_len_from_F ()
{
  double d, avg_len = 0;
  unsigned int count = 0;
  vgl_point_3d<double> P0, P1;
  assert (facemap_.size() != 0);

  //Loop through each mesh face and count bnd edge len.
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    int sz = F->vertices().size();
    assert (sz > 1);
    for (int i=0; i<sz-1; i++) {
      P0 = F->vertices(i)->pt();
      P1 = F->vertices(i+1)->pt();
      d = vgl_distance (P0, P1);
      avg_len += d;
    }
    P0 = F->vertices(0)->pt();
    P1 = F->vertices(sz-1)->pt();
    d = vgl_distance (P0, P1);
    avg_len += d;
    count += sz;
  }

  assert (count != 0);
  return avg_len / count;
}

//############################################################################

bmsh3d_mesh::bmsh3d_mesh(const bmsh3d_mesh& mesh)
: edge_id_counter_(mesh.edge_id_counter_), i_traverse_flag_(mesh.i_traverse_flag_),
b_watertight_(mesh.b_watertight_)
{
  /*// deep copy the edgemap
  vcl_map<int, bmsh3d_edge* > edgemap = mesh.edgemap_;
  vcl_map<int, bmsh3d_edge* >::iterator edge_it = edgemap.begin();
  while (edge_it != edgemap.end()) {
    // create new edges
    bmsh3d_edge* edge = edge_it->second;
    bmsh3d_vertex* s_vertex = edge->sV();
    bmsh3d_vertex* eV = edge->eV();


    bmsh3d_vertex* s = this->_new_vertex();
    bmsh3d_vertex* e = this->_new_vertex();
    s->get_pt().set(s_vertex->get_pt());
    this->add_new_edge(s, e);
    edge_it++;
  }

  // deep copy the faces
  vcl_map<int, bmsh3d_face* > facemap = mesh.facemap();
  vcl_map<int, bmsh3d_edge* >::iterator face_it = facemap.begin();
  while (face_it != facemap.end()) {
    bmsh3d_face* f = new bmsh3d_face(face_it->second);
    this->_add_face(f);
    face_it++;
  }*/
}

bmsh3d_halfedge* _connect_F_E_end (bmsh3d_face* F, bmsh3d_edge* E)
{
  //The halfedge will be deleted when the face disconnect from the F.
  bmsh3d_halfedge* halfedge = new bmsh3d_halfedge (E, F);
  //Handle the both-way connectivity of halfedge-face.
  F->_connect_HE_to_end (halfedge);
  //Handle the both-way connectivity of halfedge-edge.
  E->_connect_HE_to_end (halfedge);
  return halfedge;
}

// ----------------------------------------------------------------------------


//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int bmsh3d_ifs_mesh::_count_faces_indices_ifs() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    assert (F->vertices().size() != 0);
    unsigned int nVer = F->vertices().size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
//  Skip the unmarked face with false i_visited_
unsigned int bmsh3d_ifs_mesh::_count_visited_faces_indices_ifs() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    if (F->b_visited() == false)
      continue; //skip the unmarked F.

    unsigned int nVer = F->vertices().size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

//: Assign IFS_vidx using the vertex ids
//  for instance, used in the surface mesh reconstruction,
//  where the indices of the vertices is known.
void bmsh3d_ifs_mesh::ifs_assign_Vs_vid_by_id ()
{
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    F->_ifs_assign_Vs_vid_by_id ();
  }
}

//: Assign IFS_vid using the order of the vertex in the map
// The first vertex will have vid_ = 0 and the last in the map
// will have vid_ = (num_vertices - 1);
void bmsh3d_ifs_mesh::assign_IFS_vertex_vid_by_vertex_order()
{
  
  this->reset_vertex_traversal();
  int vid_count = 0;
  for (bmsh3d_vertex* vb = 0; this->next_vertex(vb); vid_count ++) {
    bmsh3d_vertex* vertex = (vb);
    vertex->set_vid(vid_count);
  }

  return;
}

void bmsh3d_ifs_mesh::mark_unmeshed_pts ()
{
  //Reset the unmeshed flag of each point.
  vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;
    V->set_meshed (false);
  }

  //Go through all mesh faces and mark incident points.
  vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    bmsh3d_face* F = (*fit).second;

    for (unsigned int i=0; i<F->vertices().size(); i++) {
      bmsh3d_vertex* V = F->vertices(i);
      V->set_meshed (true);
    }
  }
}

void bmsh3d_ifs_mesh::delete_unmeshed_pts ()
{
  mark_unmeshed_pts ();

  //Go through each point and delete unmeshed ones.
  vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  while (vit != vertexmap_.end()) {
    bmsh3d_vertex* V = (*vit).second;
    if (V->b_meshed() == false) {
      vit++;
      remove_vertex (V);
    }
    else
      vit++;
  }
}

// ###################################################################

double bmsh3d_mesh::get_avg_edge_len_from_F ()
{
  if (is_MHE() == false)
    return bmsh3d_ifs_mesh::get_avg_edge_len_from_F ();

  double avg_len = 0;
  unsigned int count = 0;
  assert (facemap_.size() != 0);

  //Loop through each mesh face and count bnd edge len.
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    assert (F->halfedge());
    assert (F->halfedge()->next());

    bmsh3d_halfedge* HE = F->halfedge();
    do {
      avg_len += HE->edge()->length();
      count++;      
      HE = HE->next();
    }
    while (HE != F->halfedge());
  }

  assert (count != 0);
  return avg_len / count;
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int bmsh3d_mesh::count_faces_indices() 
{
  if (is_MHE())
    return _count_faces_indices_mhe ();
  else
    return _count_faces_indices_ifs ();
}

//: Count the size of mesh faces indices for visualization using SoIndexedFaceSet
unsigned int bmsh3d_mesh::_count_faces_indices_mhe() 
{
  unsigned int totalVertices = 0;
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    vcl_vector<bmsh3d_vertex*> vertices;
    F->get_ordered_Vs (vertices);
    unsigned int nVer = vertices.size();
    nVer = nVer + 1; //the '-1' field
    totalVertices += nVer;
  }
  return totalVertices;
}

//: Add all faces' incident edges and vertices into the map.
void bmsh3d_mesh::_update_incident_Es_Vs ()
{
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;

    bmsh3d_halfedge* HE = F->halfedge();
    do {
      bmsh3d_edge* E = HE->edge();
      _add_edge (E);
      _add_vertex (E->vertices(0));
      _add_vertex (E->vertices(1));
      HE = HE->next();
    }
    while (HE != F->halfedge());
  }
}

void bmsh3d_mesh::remove_F_del_isolated_Es (bmsh3d_face* F)
{ 
  //The list of edges to delete.
  vcl_vector<bmsh3d_edge*> edges_to_del;
  bmsh3d_halfedge* HE = F->halfedge();
  do {
    bmsh3d_edge* E = HE->edge();
    if (E->halfedge()->pair() == NULL)
      edges_to_del.push_back (E);
    HE = HE->next();
  }
  while (HE != F->halfedge());

  remove_face (F);

  for (unsigned int i=0; i<edges_to_del.size(); i++) {
    remove_edge (edges_to_del[i]);
  }
}

//: Create and add a mesh face from a given set of ordered edges.
bmsh3d_face* bmsh3d_mesh::add_new_face (const vcl_vector<bmsh3d_edge*>& ordered_edges)
{
  //Create and add the new face to the structure.
  bmsh3d_face* F = _new_face();  
  _add_face (F);

  //Create the first halfedge of incidence relationship
  assert (ordered_edges.size() > 2);
  bmsh3d_halfedge* HE = new bmsh3d_halfedge (ordered_edges[0], F);
  F->set_halfedge (HE);  
  ordered_edges[0]->_connect_HE_to_end (HE); //add the HE to the edge's pair_ structure.

  //for each next_edge, create a halfedge and add the incidence connectivity.
  for (unsigned int i=1; i<ordered_edges.size(); i++) {
    bmsh3d_edge* e = ordered_edges[i];
    bmsh3d_halfedge* nextHE = new bmsh3d_halfedge (e, F);

    //add the nextHE to the edge's pair_ structure.
    e->_connect_HE_to_end (nextHE);

    //add to the circular list of halfedges.
    HE->set_next (nextHE);
    HE = nextHE;
  }
    
  //finish the circular list of halfedges.
  HE->set_next (F->halfedge());

  return F;
}

// ----------------------------------------------------------------------------
// Print a summary of mesh properties
void bmsh3d_mesh::print_summary(vcl_ostream& str)
{
  str << "\n-------------------------------------------------\n";
  str << "Mesh info \n" ;
  str << "Number of vertices = " << this->vertexmap().size() << "\n";
  str << "Number of faces = " << this->facemap().size() << "\n";
  str << "Number of edges = " << this->edgemap().size() << "\n";
}

void bmsh3d_mesh::print_topo_summary (void)
{ 
  vul_printf (vcl_cerr, "\nThe reconstructed surface mesh is:\n");

  bool b_2_manifold;
  unsigned int n_bnd_edges = count_bnd_edges (b_2_manifold);

  if (b_2_manifold)
    vul_printf (vcl_cerr, "\t- 2-manifold\n");
  else
    vul_printf (vcl_cerr, "\t- non-2-manifold\n");

  int max_sides = count_max_polygon_sides ();

  if (max_sides == 3)
    vul_printf (vcl_cerr, "\t- triangular\n");
  else
    vul_printf (vcl_cerr, "\t- non-triangular, max %u polygon sides.\n", max_sides);


  if (n_bnd_edges == 0)
    vul_printf (vcl_cerr, "\t- closed watertight surface.\n\n");
  else
    vul_printf (vcl_cerr, "\t- non-watertight, %u boundary edges.\n\n", n_bnd_edges);
}

bmsh3d_mesh* clone_mesh_ifs_3d (bmsh3d_mesh* M)
{
  //Assume M is in data structure mode of IFS, no isolated edge.
  bmsh3d_mesh* newM = new bmsh3d_mesh ();

  //Clone all vertices of M.
  //Note: use _new_vertex() to create a new vertex.
  vcl_map<int, bmsh3d_vertex*>::iterator vit = M->vertexmap().begin();
  for (; vit != M->vertexmap().end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;
    bmsh3d_vertex* newV = newM->_new_vertex (V->id());
    newV->set_pt (V->pt());
    newM->_add_vertex (newV);
  }
  newM->set_vertex_id_counter (M->vertex_id_counter());
  newM->set_free_objects_in_destructor (M->b_free_objects_in_destructor());

  //Clone all faces of M.
  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    bmsh3d_face* newF = newM->_new_face (F->id());
    for (unsigned int i=0; i<F->vertices().size(); i++) {
      int vid = F->vertices(i)->id();
      bmsh3d_vertex* newV = newM->vertexmap (vid);
      newF->_ifs_add_vertex (newV);
    }
    newM->_add_face (newF);
  }
  newM->set_face_id_counter (M->face_id_counter());

  return newM;
}

bmsh3d_face* add_F_to_M (vcl_vector<int>& vids, bmsh3d_mesh* M)
{  
  //First find or create the set of order edges of this face.
  //assumption: vids is ordered a circular way.
  vcl_vector<bmsh3d_edge*> ordered_edges;

  //Add one more vertex to the end for circulation.
  vids.push_back (vids[0]);
  for (unsigned int i=0; i<vids.size()-1; i++) {
    bmsh3d_vertex* V0 = M->vertexmap (vids[i]);
    if (V0 == NULL) { //if vertex not found, create a new vertex.
      V0 = M->_new_vertex (vids[i]);
      M->_add_vertex (V0);
    }
    bmsh3d_vertex* V1 = M->vertexmap (vids[i+1]);
    if (V1 == NULL) { //if vertex not found, create a new vertex.
      V1 = M->_new_vertex (vids[i+1]);
      M->_add_vertex (V1);
    }

    bmsh3d_edge* E = E_sharing_2V (V0, V1);
    if (E == NULL) //if edge not found, create a new edge.
      E = M->add_new_edge (V0, V1);

    ordered_edges.push_back (E);
  }

  //Create the face from the set of ordered edges.
  bmsh3d_face* newF = M->add_new_face (ordered_edges);
  return newF;
}

bmsh3d_face* add_F_to_M_check_topo (vcl_vector<int>& vids, bmsh3d_mesh* M)
{
  //First find or create the set of order edges of this face.
  //assumption: vids is ordered a circular way.
  vcl_vector<bmsh3d_edge*> ordered_edges;

  //Add one more vertex to the end for circulation.
  vids.push_back (vids[0]);

  //The topo. error only occur for existing edges or vertices.
  VTOPO_TYPE vt;

  for (unsigned int i=0; i<vids.size()-1; i++) {
    bmsh3d_vertex* V0 = M->vertexmap (vids[i]);
    if (V0 == NULL) { //if vertex not found, create a new vertex.
      V0 = M->_new_vertex (vids[i]);
      M->_add_vertex (V0);
    }
    else { //Check V0 for non-2-manifold 1-ring
      vt = V0->detect_vtopo_type();
      if (vt==VTOPO_2_MANIFOLD_1RING || vt==VTOPO_NON_MANIFOLD_1RING)
        return NULL;
    }

    bmsh3d_vertex* V1 = M->vertexmap (vids[i+1]);
    if (V1 == NULL) { //if vertex not found, create a new vertex.
      V1 = M->_new_vertex (vids[i+1]);
      M->_add_vertex (V1);
    }
    else { //Check V1 for non-2-manifold 1-ring
      vt = V1->detect_vtopo_type();
      if (vt==VTOPO_2_MANIFOLD_1RING || vt==VTOPO_NON_MANIFOLD_1RING)
        return NULL;
    }

    bmsh3d_edge* E = E_sharing_2V (V0, V1);
    if (E == NULL) { //if edge not found, create a new edge.
      E = M->add_new_edge (V0, V1);
    }
    else { //Check E for non-2-manifold topology.
      if (E->n_incident_Fs() > 1)
        return NULL;
    }

    ordered_edges.push_back (E);
  }

  //Create the face from the set of ordered edges.
  bmsh3d_face* newF = M->add_new_face (ordered_edges);
  return newF;
}

void add_M_faces_to_IFSset (bmsh3d_mesh* M, vcl_vector<vcl_vector<int> >& faces)
{
  vcl_map<int, bmsh3d_face*>::iterator fit = M->facemap().begin();
  for (; fit != M->facemap().end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    vcl_vector<int> vids;
    F->get_ordered_V_ids (vids);
    faces.push_back (vids);
  }
}

// ----------------------------------------------------------------------------
void bmsh3d_mesh_print_object_size ()
{
  vul_printf (vcl_cerr, "\n\n");
  vcl_vector<bmsh3d_edge*> tmp_vector;
  vcl_cout << "size of vcl_vector: "<< sizeof(tmp_vector) << vcl_endl;  
  vcl_list<bmsh3d_edge*> tmp_list;
  vcl_cout << "size of vcl_list: "<< sizeof(tmp_list) << vcl_endl;
  vcl_set<bmsh3d_edge*> tmp_set;
  vcl_cout << "size of vcl_set: "<< sizeof(tmp_set) << vcl_endl;
  vcl_set<bmsh3d_edge*> tmp_map;
  vcl_cout << "size of vcl_map: "<< sizeof(tmp_map) << vcl_endl;
  vul_printf (vcl_cerr, "\n");

  vul_printf (vcl_cerr, "    Object          Size (bytes)\n");
  vul_printf (vcl_cerr, "------------------------------------\n");
  vul_printf (vcl_cerr, "vispt_elm                %3d\n", sizeof (vispt_elm));
  vul_printf (vcl_cerr, "bmsh3d_vertex       --  %3d\n", sizeof (bmsh3d_vertex));
  vul_printf (vcl_cerr, "\n");
  vul_printf (vcl_cerr, "bmsh3d_halfedge     --  %3d\n", sizeof (bmsh3d_halfedge));
  vul_printf (vcl_cerr, "bmsh3d_edge         --  %3d\n", sizeof (bmsh3d_edge));
  vul_printf (vcl_cerr, "\n");
  vul_printf (vcl_cerr, "bmsh3d_face         --  %3d\n", sizeof (bmsh3d_face));
  vul_printf (vcl_cerr, "\n");
  
  vul_printf (vcl_cerr, "bmsh3d_pt_set           %3d\n", sizeof (bmsh3d_pt_set));
  vul_printf (vcl_cerr, "bmsh3d_ifs_mesh         %3d\n", sizeof (bmsh3d_ifs_mesh));
  vul_printf (vcl_cerr, "bmsh3d_mesh             %3d\n", sizeof (bmsh3d_mesh));
}

bool bmsh3d_mesh::is_2_manifold ()
{
  vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    bmsh3d_edge* edge = (*it).second;
    if (edge->n_incident_Fs() > 2)
      return false;
  }
  return true;
}

unsigned int bmsh3d_mesh::count_max_polygon_sides ()
{
  unsigned int max_sides = 0;
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;

    unsigned int sides = F->n_incident_Es();
    if (sides > max_sides)
      max_sides = sides;
  }

  return max_sides;
}

//: Return number of edges in traversing the IFS data structure.
//  Note that an internal edge can be used twice.
unsigned int bmsh3d_mesh::count_ifs_dup_edges ()
{
  unsigned int count = 0;
  vcl_map<int, bmsh3d_face*>::iterator it = facemap_.begin();
  for (; it != facemap_.end(); it++) {
    bmsh3d_face* F = (*it).second;
    count += F->vertices().size();
  }
  return count;
}

//: return total boundary edges
//  also determine if this mesh is watertight or not.
unsigned int bmsh3d_mesh::count_bnd_edges (bool& b_2_manifold)
{
  b_2_manifold = true;
  unsigned int bnd_edges = 0;

  vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    bmsh3d_edge* edge = (*it).second;

    unsigned int count = edge->n_incident_Fs();
    if (count > 2)
      b_2_manifold = false;
    if (count < 2)
      bnd_edges++;
  }
  b_watertight_ = (bnd_edges == 0);
  return bnd_edges;
}

double bmsh3d_mesh::get_avg_edge_len ()
{
  unsigned int  count_edge = 0;
  double        sum_length = 0;

  assert (edgemap_.size() != 0);
  vcl_map<int, bmsh3d_edge*>::iterator it = edgemap_.begin();
  for (; it != edgemap_.end(); it++) {
    bmsh3d_edge* edge = (*it).second;
    
    double len = edge->length();
    count_edge++;
    sum_length += len;
  }

  double avg_len = sum_length / count_edge;
  return avg_len;
}

//: Merge two faces who sharing an edge together.
//  Only valid if E is a 2-manifold edge.
//  Delete edge E and add all remaining halfedges of F2 to F1, and then delete E and F2.
//  Result of merged face is in F1.
//
void bmsh3d_mesh::m2_mesh_merge_face (bmsh3d_face* F1, bmsh3d_face* F2 ,bmsh3d_edge* E)

{
  assert (F1 != F2);
  vcl_cout << "mesh_merge_faces " << F1->id() << " with " << F2->id() << vcl_endl;

  bmsh3d_vertex* Vs = E->sV();
  bmsh3d_vertex* Ve = E->eV();
  bmsh3d_halfedge* HE1 = E->get_HE_of_F (F1);
  bmsh3d_halfedge* HE2 = E->get_HE_of_F (F2);

  bmsh3d_halfedge* F1_HE_head = F1->find_other_HE (Vs, HE1);
  bmsh3d_halfedge* F1_HE_tail = F1->find_other_HE (Ve, HE1);
  //Swap the head & tail if necessary
  if (F1_HE_head->next() != HE1) {
    assert (F1_HE_tail->next() == HE1);
    bmsh3d_halfedge* temp = F1_HE_tail;
    F1_HE_tail = F1_HE_head;
    F1_HE_head = temp;
  }

  //Traverse all halfedges of F2 and add to F1 in order.
  bmsh3d_halfedge* HE = HE2->next();
  while (HE != HE2) {
    F1_HE_head->set_next (HE);
    HE->set_face (F1);
    F1_HE_head = HE;
    HE = HE->next();
  }
  F1_HE_head->set_next (F1_HE_tail);

  //Delete F2. Note that most of F2's halfedges now belongs to F1.
  F2->set_halfedge (HE2);
  HE2->set_next (NULL);
  remove_face (F2);

  //Remove HE1: the other incidence of edge E.
  E->_disconnect_HE (HE1);
  delete HE1;
  assert (E->halfedge() == NULL);
  remove_edge (E);

  //Sort F1's HE list.
  F1->set_halfedge (F1_HE_tail);
  F1->_sort_HEs_circular ();
  F1->_ifs_clear_vertices ();
}

//########################################################################

//: For 2-manifold mesh only.
//  This function fix each face's halfedges in a consistent order.
//  Pick one face and fix the orientation of its halfedges using right hand rule on it.
//  propagation the orientation to all adjacent faces
//  result: for a 2-manifold mesh, all edges in general has two incident faces
//          (otherwise this edge is on the boundary of the mesh)
//          after this orientation fix, for all edges, the 2 sides (halfedges) of it 
//          on both adjacent faces has different direction.
//  Parameter:
//    mesh:         the mesh under work
//    sfaceid:      the id of the starting face
//    b_flip_sface: flip the orientation of the starting face or not.
void manifold_fix_faces_orientation (bmsh3d_mesh* mesh, int sfaceid, bool b_use_e_vertex_as_next)
{
  mesh->reset_traverse ();
  
  //: fix the starting_face
  bmsh3d_face* starting_face = mesh->facemap (sfaceid);
  bmsh3d_halfedge* startHE = starting_face->halfedge(); 
  if (b_use_e_vertex_as_next)
    starting_face->set_orientation (startHE, startHE->edge()->eV());
  else
    starting_face->set_orientation (startHE, startHE->edge()->sV());

  starting_face->set_i_visited (mesh->i_traverse_flag());

  //: propagation of the orientation to all other faces
  //  the front here is the halfedge with associated faces to propagate.
  vcl_queue<bmsh3d_halfedge*> he_queue;

  //: put all adjacent faces into the queue
  bmsh3d_halfedge* HE = startHE;
  do {
    bmsh3d_edge* edge = HE->edge();
    //: assume 2-manifold mesh
    if (edge->halfedge()->face() != starting_face)
      he_queue.push (edge->halfedge());
    else if (edge->halfedge()->pair())
      he_queue.push (edge->halfedge()->pair());

    HE = HE->next();
  }
  while (HE != startHE);

  //: the main while loop.
  while (!he_queue.empty()) {
    //: pop one
    bmsh3d_halfedge* front_he = he_queue.front();
    he_queue.pop();

    //: find the already oriented next_v on the other side
    bmsh3d_halfedge* sorted_he = front_he->pair();
    bmsh3d_halfedge* sorted_he_next = sorted_he->next();
    
    bmsh3d_vertex* next_v = incident_V_of_Es (sorted_he->edge(), sorted_he_next->edge());
    next_v = front_he->edge()->other_V (next_v);

    bmsh3d_face* front_face = front_he->face();
    //: this check is necessary. note that a face can be traversed more than once.
    if (front_face->is_visited (mesh->i_traverse_flag()))
      continue;

    //: fix orientation of it
    front_face->set_orientation (front_he, next_v);

    front_face->set_i_visited (mesh->i_traverse_flag());

    //: put all non-visited face in the queue
    bmsh3d_halfedge* HE = front_face->halfedge();
    do {
      bmsh3d_edge* edge = HE->edge();
      //: assume 2-manifold mesh
      if (edge->halfedge()->face() != front_face) {
        if (edge->halfedge()->face()->is_visited (mesh->i_traverse_flag()) == false)
          he_queue.push (edge->halfedge());
      }
      else if (edge->halfedge()->pair()) {
        bmsh3d_face* F = edge->halfedge()->pair()->face();
        assert (F != front_face);
        if (!F->is_visited (mesh->i_traverse_flag()))
          he_queue.push (edge->halfedge()->pair());
      }

      HE = HE->next();
    }
    while (HE != front_face->halfedge());
  }
}
