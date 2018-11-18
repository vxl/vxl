// This is brl/bbas/bmsh3d/bmsh3d_textured_mesh_mc.cxx
//---------------------------------------------------------------------
#include "bmsh3d_textured_mesh_mc.h"
//:
// \file
// \brief Mesh
//
// \author Daniel Crispell
// \date   March 20, 2007
//
// \verbatim
//  Modifications
//   <none>
// \endverbatim
//
//-------------------------------------------------------------------------

#include "bmsh3d_mesh_mc.h"
#include "bmsh3d_textured_face_mc.h"
#include "bmsh3d_vertex.h"

//: create a textured mesh from a non-textured mesh
bmsh3d_textured_mesh_mc::bmsh3d_textured_mesh_mc(bmsh3d_mesh_mc* mesh): bmsh3d_mesh_mc()
{
  // shallow copy the vertices
  std::map<int, bmsh3d_vertex* > vertices = mesh->vertexmap();
  std::map<int, bmsh3d_vertex* >::iterator v_it;
  for (v_it = vertices.begin(); v_it != vertices.end(); v_it++) {
    bmsh3d_vertex* vertex = v_it->second;
    this->_add_vertex(vertex);
  }

  // shallow copy the edgemap
  std::map<int, bmsh3d_edge* > edgemap = mesh->edgemap();
  std::map<int, bmsh3d_edge* >::iterator edge_it;
  for (edge_it = edgemap.begin(); edge_it != edgemap.end(); edge_it++) {
    // create new edges
    bmsh3d_edge* edge = edge_it->second;
    this->add_edge_incidence (edge);
  }

  // deep copy the faces (need to create textured faces)
  std::map<int, bmsh3d_face* > fmap = mesh->facemap();
  std::map<int, bmsh3d_face* >::iterator face_it;
  for (face_it = fmap.begin(); face_it != fmap.end(); face_it++) {

    auto* face = (bmsh3d_face_mc*) face_it->second;
    face->_sort_HEs_circular();
    face->_ifs_clear_vertices ();

    bmsh3d_textured_face_mc* f = this->_new_face();
    bmsh3d_halfedge* he = face->halfedge();
    bmsh3d_halfedge* HE = he;

    // clone the half edges
    do {
      bmsh3d_edge* edge = HE->edge();
#ifdef DEBUG
      std::cout << edge->id() << std::endl;
#endif
      auto* new_he = new bmsh3d_halfedge(this->edgemap(edge->id()), f);
      f->connect_E_to_end(new_he->edge());
      HE = HE->next();
    }while (HE != he);

    this->_add_face(f);
    // clone the inner faces
    if (face->size() > 0) {
      std::map<int, bmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      auto it = inner_faces.begin();
      while (it != inner_faces.end()) {
        std::vector<bmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        bmsh3d_face* inner_face = copy_inner_face(incident_edges, mesh);
        f->add_mc_halfedge((bmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
  }
}


bmsh3d_textured_mesh_mc* bmsh3d_textured_mesh_mc::clone() const
{
  auto* mesh = new bmsh3d_textured_mesh_mc();

  // deep copy the vertices
  std::map<int, bmsh3d_vertex* > vertices = this->vertexmap_;
  auto v_it = vertices.begin();
  while (v_it != vertices.end()) {
    auto* vertex = (bmsh3d_vertex*) v_it->second;
    auto* v = (bmsh3d_vertex*) mesh->_new_vertex();
    v->set_pt(vertex->get_pt());
    mesh->_add_vertex(v);
    v_it++;
  }

 // deep copy the edgemap
  std::map<int, bmsh3d_edge* > edgemap = this->edgemap_;
  auto edge_it = edgemap.begin();
  while (edge_it != edgemap.end()) {
    // create new edges
    bmsh3d_edge* edge = edge_it->second;
#ifdef DEBUG
    std::cout << "old edge id=" << edge->id() << std::endl;
#endif
    auto* new_edge = // mesh->_new_edge
      new bmsh3d_edge((bmsh3d_vertex*) mesh->vertexmap(edge->sV()->id()),
                      (bmsh3d_vertex*) mesh->vertexmap(edge->eV()->id()),
                      edge_it->first);
#if 0
    std::cout << "  new edge id=" << new_edge->id() << std::endl
             << " v1=" << ((bmsh3d_vertex*)new_edge->sV())->get_pt()
             << " v2=" << ((bmsh3d_vertex*)new_edge->eV())->get_pt()
             << std::endl;
#endif // 0
    mesh->add_edge_incidence (new_edge);
    edge_it++;
  }

  // deep copy the faces
  std::map<int, bmsh3d_face* > fmap = this->facemap_;
  auto face_it = fmap.begin();
  while (face_it != fmap.end()) {
    auto* face = (bmsh3d_textured_face_mc*) face_it->second;
#ifdef DEBUG
    std::cout << "old face id=" << face->id() << std::endl;
#endif
    face->_sort_HEs_circular();
    face->_ifs_clear_vertices ();
    bmsh3d_textured_face_mc* f = mesh->_new_face();
    bmsh3d_halfedge* he = face->halfedge();
    bmsh3d_halfedge* HE = he;

    // copy the texture coordinates

    f->tex_coords().insert(face->tex_coords().begin(), face->tex_coords().end());

    // clone the half edges
    do {
      bmsh3d_edge* edge = HE->edge();
#ifdef DEBUG
      std::cout << edge->id() << std::endl;
#endif
      auto* new_he = new bmsh3d_halfedge(mesh->edgemap(edge->id()), f);
      f->connect_E_to_end(new_he->edge());
      HE = HE->next();
    }while (HE != he);

    mesh->_add_face(f);
    // clone the inner faces
    if (face->size() > 0) {
      std::map<int, bmsh3d_halfedge*> inner_faces = face->get_mc_halfedges();
      auto it = inner_faces.begin();
      while (it != inner_faces.end()) {
        std::vector<bmsh3d_edge*> incident_edges;
        face->get_mc_incident_edges(it->second, incident_edges);
        bmsh3d_face* inner_face = copy_inner_face(incident_edges, mesh);
        f->add_mc_halfedge((bmsh3d_halfedge *)inner_face->halfedge());
        it++;
      }
    }
    face_it++;
  }
  return mesh;
}
