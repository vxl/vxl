// This is brl/bbas/bmsh3d/bmsh3d_mesh_conn_recov.cxx
//:
// \file
// \brief Indexed Face Set to Half Edge connectivity recovery
//
// \author
//  MingChing Chang  June 20, 2005
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include <vcl_iostream.h>
#include <vcl_cassert.h>
#include <vcl_algorithm.h>
#include <vul/vul_printf.h>
#include "bmsh3d_mesh.h"

// ###########################################################################
//    MESH CONNECTIVITY RECOVERY (INDEXED-FACE-SET --> MODIFIED-HALF-EDGE)
// ###########################################################################

void bmsh3d_mesh::IFS_to_MHE ()
{
  //Loop through all face F and put F into its V's incident face list.
  //This V-F incidence will be cleared after MHE is built.
  vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    for (unsigned int j=0; j<F->vertices().size(); j++) {
      bmsh3d_vertex* V = F->vertices(j);
      V->add_F ((void*) F);
    }
  }

  IFS_to_MHE_build_edges ();
  vul_printf (vcl_cerr, "IFS_to_MHE(): mesh #v: %d, #f: %d, #e: %d\n",
              vertexmap_.size(), facemap_.size(), edgemap_.size());
}

void bmsh3d_mesh::IFS_to_MHE_build_edges (const bool skip_v0)
{
  vul_printf (vcl_cerr, "  IFS_to_MHE_build_edges():\n");
  assert (is_MHE() == false);

  //For each V, try to locate the other V sharing an edge via F.
  vul_printf (vcl_cerr, "    Constructing mesh edges: ");
  int prev_per = 0;
  vcl_map<int, bmsh3d_vertex*>::iterator nit = vertexmap_.begin();
  for (int count=0; nit != vertexmap_.end(); nit++, count++) {
    bmsh3d_vertex* V = (*nit).second;
    float percentage = (float) (count * 100.0 / vertexmap_.size());
    int per = static_cast<int>(vcl_ceil (percentage));
    if (per % 10 ==0 && per != prev_per)
      vul_printf (vcl_cerr, "%2d%% ", per);
    prev_per = per;

    for (bmsh3d_ptr_node* cur = V->F_list(); cur != NULL; cur = cur->next()) {
      bmsh3d_face* F = (bmsh3d_face*) cur->ptr();

      //1) The prev node
      bmsh3d_vertex* prevV = F->_ifs_prev_V (V);
      if (skip_v0 && prevV->id()==0) //ignore the infinity node
        continue;

      //If link between prevV <-> v not exist, create one.
      bmsh3d_edge* E = E_sharing_2V (prevV, V);
      if (E == NULL)
        E = add_new_edge (prevV, V);

      //Connect the face to the edge through the half-edge
      if (!E->is_F_incident (F))
        _connect_F_E_end (F, E);

      //2) The next node, ignore the infinity node
      bmsh3d_vertex* nextV = F->_ifs_next_V (V);
      if (skip_v0 && nextV->id()==0)
        continue;

      //If link between v <-> nextV not exist, create one.
      E = E_sharing_2V (V, nextV);
      if (E == NULL)
        E = add_new_edge (V, nextV);

      //Connect the face to the edge through the half-edge
      if (!E->is_F_incident (F))
        _connect_F_E_end (F, E);
    }

    //Now clean up V's ptr's.
    V->clear_F_list();
  }

  sort_halfedges_for_all_faces ();
}

//: Brute-forcely build MHE from IFS
void bmsh3d_mesh::IFS_to_MHE_bf (const bool skip_v0)
{
  vul_printf (vcl_cerr, "\n  IFS_to_MHE_bf():\n");

  //For each V, try to locate the other V sharing an edge via F.
  vul_printf (vcl_cerr, "    Constructing mesh edges: ");
  int prev_per = 0;
  vcl_map<int, bmsh3d_vertex*>::iterator nit = vertexmap_.begin();
  for (int count=0; nit != vertexmap_.end(); nit++, count++) {
    bmsh3d_vertex* V = (*nit).second;
    float percentage = (float) (count * 100.0 / vertexmap_.size());
    int per = static_cast<int>(vcl_ceil (percentage));
    //if (per % 10 ==0 && per != prev_per)
    if (per != prev_per)
      vul_printf (vcl_cerr, "%2d%% ", per);
    prev_per = per;

    //Loop through each incident face for this V
    vcl_set<void*> V_incident_Fs;
    V_incident_Fs.clear();

    vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
    for (; fit != facemap_.end(); fit++) {
      bmsh3d_face* F = (*fit).second;
      for (unsigned int i=0; i<F->vertices().size(); i++) {
        if (F->vertices(i) == V)
          V_incident_Fs.insert (F);
      }
    }

    vcl_set<void*>::iterator it = V_incident_Fs.begin();
    for (; it != V_incident_Fs.end(); it++) {
      bmsh3d_face* F = (bmsh3d_face*) (*it);

      //1) The prev node
      bmsh3d_vertex* prevV = F->_ifs_prev_V (V);
      if (skip_v0 && prevV->id()==0) //ignore the infinity node
        continue;

      //If link between prevV <-> v not exist, create one.
      bmsh3d_edge* E = E_sharing_2V (prevV, V);
      if (E == NULL)
        E = add_new_edge (prevV, V);

      //Connect the face to the edge through the half-edge
      if (!E->is_F_incident (F))
        _connect_F_E_end (F, E);

      //2) The next node, ignore the infinity node
      bmsh3d_vertex* nextV = F->_ifs_next_V (V);
      if (skip_v0 && nextV->id()==0)
        continue;

      //If link between v <-> nextV not exist, create one.
      E = E_sharing_2V (V, nextV);
      if (E == NULL)
        E = add_new_edge (V, nextV);

      //Connect the face to the edge through the half-edge
      if (!E->is_F_incident (F))
        _connect_F_E_end (F, E);
    }

    //Now clean up V's ptr's.
    V->clear_F_list();
  }

  sort_halfedges_for_all_faces ();
}

void bmsh3d_mesh::sort_halfedges_for_all_faces ()
{
  if (facemap_.size() == 0)
    return;

  vul_printf (vcl_cerr, "\n      sort_halfedges_for_all_faces(): %u faces.\n", facemap_.size());
  assert (is_MHE());

  //Go through each face, re-sort the list of halfedges in a circular way
  vcl_map<int, bmsh3d_face*>::iterator pit = facemap_.begin();
  for (; pit != facemap_.end(); pit++) {
    bmsh3d_face* F = (*pit).second;
    F->_sort_HEs_circular ();
    F->_ifs_clear_vertices ();
  }
}

void bmsh3d_mesh::build_face_IFS ()
{
  vul_printf (vcl_cerr, "bmsh3d_mesh::build_face_IFS()\n");
  assert (is_MHE());

  vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    F->_ifs_track_ordered_vertices ();
  }
  vul_printf (vcl_cerr, "\tdone.\n");
}

//: Build Indexed-Face-Set from the MHE representation.
//  For faces: Go through each face, re-sort the list of halfedges in a circular way.
//  For vertices: Add all incident vertices of faces if there are not in mesh's vertexmap.
//
void bmsh3d_mesh::build_IFS_mesh ()
{
  assert (is_MHE());

  vcl_map<int, bmsh3d_face*>::iterator fit = facemap_.begin();
  for (; fit != facemap_.end(); fit++) {
    bmsh3d_face* F = (*fit).second;
    F->_ifs_track_ordered_vertices ();

    for (unsigned int j=0; j<F->vertices().size(); j++) {
      bmsh3d_vertex* V = F->vertices(j);
      if (contains_V (V->id()) == false)
        _add_vertex (V);
    }
  }
}

void bmsh3d_mesh::clean_IFS_mesh ()
{
  assert (is_MHE());

  //Go through each face and clear the F.vertices[]
  vcl_map<int, bmsh3d_face*>::iterator pit = facemap_.begin();
  for (; pit != facemap_.end(); pit++) {
    bmsh3d_face* F = (*pit).second;
    F->vertices().clear();
  }
}

void bmsh3d_mesh::MHE_to_IFS ()
{
  vul_printf (vcl_cerr, "  MHE_to_IFS().\n");
  build_face_IFS ();

  //Delete all mesh edges and halfedges.

  //Go through all mesh faces and
  vcl_map<int, bmsh3d_face*>::iterator pit = facemap_.begin();
  for (; pit != facemap_.end(); pit++) {
    bmsh3d_face* F = (*pit).second;
    _delete_HE_chain (F->halfedge());
  }

  //Delete all mesh edges.
  vcl_map<int, bmsh3d_edge*>::iterator eit = edgemap_.begin();
  while (eit != edgemap_.end()) {
    bmsh3d_edge* E = (*eit).second;
    remove_edge (E);
    eit = edgemap_.begin();
  }

  //Delete all mesh vertex's pointer to edges.
  vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;
    V->clear_incident_E_list();
    V->clear_F_list();
  }
}

bool edges_id_less (const bmsh3d_edge* E1, const bmsh3d_edge* E2)
{
  return E1->id() < E2->id();
}

void bmsh3d_mesh::sort_V_incident_Es ()
{
  vcl_map<int, bmsh3d_vertex*>::iterator vit = vertexmap_.begin();
  for (; vit != vertexmap_.end(); vit++) {
    bmsh3d_vertex* V = (*vit).second;

    vcl_vector<bmsh3d_edge*> edges;
    unsigned int count = 0;
    bmsh3d_ptr_node* cur = (bmsh3d_ptr_node*) V->E_list();
    for (; cur != NULL; cur = cur->next()) {
      bmsh3d_edge* E = (bmsh3d_edge*) cur->ptr();
      edges.push_back (E);
      count++;
    }
    assert (edges.size() == count);

    vcl_sort (edges.begin(), edges.end(), edges_id_less);

    V->clear_incident_E_list ();

    for (int j=int(edges.size()-1); j>=0; j--)
      V->add_incident_E (edges[j]);
  }
}

#if 0 // commented out
bool bmsh3d_mesh::valid_conn ()
{
  bool result = bmsh3d_ifs_mesh::valid_conn ();
  if (result == false)
    return result;

  // go through each linkElm and validate the halfedge list and incident faces
  vcl_map<int, bmsh3d_edge*>::iterator lit = edgemap_.begin();
  for (; lit != edgemap_.end(); lit++) {
    bmsh3d_edge* edge = (*lit).second;

    bmsh3d_halfedge* he = edge->halfedge();
    do { he = he->pair(); }
    while (he != edge->halfedge() && he != NULL );
  }

  return result; // true
}
#endif // 0
